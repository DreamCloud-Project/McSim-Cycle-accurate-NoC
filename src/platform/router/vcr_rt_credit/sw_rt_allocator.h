/*
 * sw_allocator.h
 *
 *  Created on: Nov 15, 2010
 *      Author: Anh Tran
 *
 *  Switch allocator
 *
 *  Revised:
 *  	+ 2010.11.18:
 *  		- has 2 output grant signals using these of sa_stage2 arbiters
 *  		- use out_vc_remain_reg signals (credit-based flow control)
 *  	+ 2010.11.19:
 *  		- req_adusted depends on remain_reg, plus, minus
 */

#ifndef SW_RT_ALLOCATOR_H_
#define SW_RT_ALLOCATOR_H_

#include <systemc.h>

#include "../vcr_rt_credit/support_modules/sa_stage1_rt.h"
#include "../vcr_rt_credit/support_modules/sa_stage2_rt.h"
#include "../../MemoryProfiled.h"

using namespace std;

template<int N_VCs>
#ifdef MEM_PROF
class SWRTAllocator: public sc_module, MemoryProfiled<SWRTAllocator<N_VCs> > {
#else
class SWRTAllocator: public sc_module {
#endif

public:
//template<int N_VCs>
//SC_MODULE (SWRTAllocator) {
	sc_in<int> in_vc_state_reg[N_ROUTER_PORTS][N_VCs];
	sc_in<int> out_port_req_reg[N_ROUTER_PORTS][N_VCs];
	sc_in<bool> tail_reg[N_ROUTER_PORTS][N_VCs];

//	sc_in <bool> out_full_reg[N_ROUTER_PORTS][N_VCs];	// ouf_full registered
	sc_in<int> out_vc_remain_reg[N_ROUTER_PORTS][N_VCs]; // the number of remained entries in each output VC
	sc_in<bool> out_vc_remain_minus[N_ROUTER_PORTS][N_VCs];
	sc_in<bool> out_vc_remain_plus[N_ROUTER_PORTS][N_VCs];

	sc_in<int> sa_priority_stage1_reg[N_ROUTER_PORTS];
	sc_in<int> sa_priority_stage2_reg[N_ROUTER_PORTS];

//	sc_out <int> sa_grant[N_ROUTER_PORTS];	// = indexi = [pi][vi]
	sc_out<int> sa_grant_in_port[N_ROUTER_PORTS];	// = pi
	sc_out<int> sa_grant_in_vc[N_ROUTER_PORTS];	// = vi

	sc_out<int> sa_priority_stage1_new[N_ROUTER_PORTS];	// update priorities
	sc_out<int> sa_priority_stage2_new[N_ROUTER_PORTS];

	//====== signals
	// adjust request signals according to tail_reg, full_reg signals
	sc_signal<bool> sa_req_adjusted[N_ROUTER_PORTS][N_VCs];
	sc_signal<int> sa_grant_stage1[N_ROUTER_PORTS];

	//====== modules
	// arbiters at crossbar inputs
	SAStage1RT<N_VCs> *arbiter_stage1[N_ROUTER_PORTS];
	// arbiters at crossbar outputs
	SAStage2RT<N_VCs> *arbiter_stage2[N_ROUTER_PORTS];

	//======= processes
	void sa_req_adjusted_process();	// adjust request signals

	void sa_priority_new_process();	// update priorities

	SC_CTOR (SWRTAllocator) {

		// adjust request signals
		SC_METHOD(sa_req_adjusted_process);
		for (int pi = 0; pi < N_ROUTER_PORTS; pi++) {
			for (int vi = 0; vi < N_VCs; vi++) {
				sensitive << out_port_req_reg[pi][vi]
						<< in_vc_state_reg[pi][vi];
				sensitive << tail_reg[pi][vi];
			}
		}
		for (int po = 0; po < N_ROUTER_PORTS; po++) {
			for (int vo = 0; vo < N_VCs; vo++) {
				sensitive << out_vc_remain_reg[po][vo];
				sensitive << out_vc_remain_plus[po][vo];
				sensitive << out_vc_remain_minus[po][vo];
			}
		}

		// arbiters at stage1
		string arbiter_stage1_name = "";
		for (int pi = 0; pi < N_ROUTER_PORTS; pi++) {
			//arbiter_stage1_name = "SW_Arbiter_Stage1[" + int_to_str(pi) + "]";
			arbiter_stage1[pi] = new SAStage1RT<N_VCs>(
					arbiter_stage1_name.data());

			arbiter_stage1[pi]->in_port = pi;
			for (int vi = 0; vi < N_VCs; vi++) {
				arbiter_stage1[pi]->sa_req_adjusted[vi](
						sa_req_adjusted[pi][vi]);
			}
			arbiter_stage1[pi]->sa_priority_stage1_reg(
					sa_priority_stage1_reg[pi]);
			arbiter_stage1[pi]->sa_grant_stage1(sa_grant_stage1[pi]);
		}

		// arbiters at stage2
		string arbiter_stage2_name = "";
		for (int po = 0; po < N_ROUTER_PORTS; po++) {
			//arbiter_stage2_name = "SW_Arbiter_Stage2[" + int_to_str(po) + "]";
			arbiter_stage2[po] = new SAStage2RT<N_VCs>(
					arbiter_stage2_name.data());

			arbiter_stage2[po]->out_port = po;
			for (int pi = 0; pi < N_ROUTER_PORTS; pi++) {
				arbiter_stage2[po]->sa_grant_stage1[pi](sa_grant_stage1[pi]);
				for (int vi = 0; vi < N_VCs; vi++) {
					arbiter_stage2[po]->out_port_req_reg[pi][vi](
							out_port_req_reg[pi][vi]);
				}
			}
			arbiter_stage2[po]->sa_priority_stage2_reg(
					sa_priority_stage2_reg[po]);
			arbiter_stage2[po]->sa_grant_stage2_in_port(sa_grant_in_port[po]);
			arbiter_stage2[po]->sa_grant_stage2_in_vc(sa_grant_in_vc[po]);
		}

		// update priorities
		SC_METHOD(sa_priority_new_process);
		for (int pi = 0; pi < N_ROUTER_PORTS; pi++) {
			sensitive << sa_priority_stage1_reg[pi];
			sensitive << sa_grant_stage1[pi];
		}
		for (int po = 0; po < N_ROUTER_PORTS; po++) {
			sensitive << sa_priority_stage2_reg[po];
			sensitive << sa_grant_in_port[po];
		}
	}

private:
	void sa_priority_update_roundrobin();
	void sa_priority_update_iSLIP();
	void sa_priority_update_winner_take_all();
	void sa_priority_prio();
};

#endif /* SW_ALLOCATOR_H_ */

//=========================================
//============= function implementation

/*
 * adjust request signals
 */
template<int N_VCs>
void SWRTAllocator<N_VCs>::sa_req_adjusted_process() {
	for (int pi = 0; pi < N_ROUTER_PORTS; pi++) {
		for (int vi = 0; vi < N_VCs; vi++) {
			if (out_port_req_reg[pi][vi].read() >= 0
					&& in_vc_state_reg[pi][vi].read() >= 0
					&& !tail_reg[pi][vi].read()) {
				int po = out_port_req_reg[pi][vi].read();
				int vo = in_vc_state_reg[pi][vi].read();
				if ((out_vc_remain_reg[po][vo].read() > 1)
						|| (out_vc_remain_reg[po][vo].read() == 1
								&& !out_vc_remain_minus[po][vo].read())
						|| (out_vc_remain_reg[po][vo].read() == 1
								&& out_vc_remain_minus[po][vo].read()
								&& out_vc_remain_plus[po][vo].read())
						|| (out_vc_remain_reg[po][vo].read() == 0
								&& !out_vc_remain_minus[po][vo].read()
								&& out_vc_remain_plus[po][vo].read()))// has some idle entries in the out vc [po][vo]
					sa_req_adjusted[pi][vi].write(1);
				else
					sa_req_adjusted[pi][vi].write(0);
			} else {
				sa_req_adjusted[pi][vi].write(0);
			}
		}
	}
}

/*
 * update priorities based on allocation algorithms
 */
template<int N_VCs>
void SWRTAllocator<N_VCs>::sa_priority_new_process() {
	switch (RouterParameter::sa_type) {
	case (SA_ROUNDROBIN):
		sa_priority_update_roundrobin();
		break;
	case (SA_iSLIP):
		sa_priority_update_iSLIP();
		break;
	case (SA_WINNER_TAKE_ALL):
		sa_priority_update_winner_take_all();
		break;
	case (SA_PRIO):
		sa_priority_prio();
		break;
	default:
		cerr << "Unknown switch allocation policy: " << RouterParameter::sa_type
				<< endl;
		exit(-1);
	}

}

/*
 * update priorities using round-robin algorithm
 */
template<int N_VCs>
void SWRTAllocator<N_VCs>::sa_priority_update_roundrobin() {

	// update priority for arbiters in the stage1
	for (int pi = 0; pi < N_ROUTER_PORTS; pi++) {
		if (sa_grant_stage1[pi].read() >= 0) {	// granted
			int tmp = sa_grant_stage1[pi].read() + 1;	// update
			sa_priority_stage1_new[pi].write(tmp % N_VCs);// modulo of num of input VCs
		} else {
			sa_priority_stage1_new[pi].write(sa_priority_stage1_reg[pi].read());// keep the last priority
		}
	}

	// update priority for arbiters in the stage2
	for (int po = 0; po < N_ROUTER_PORTS; po++) {
		if (sa_grant_in_port[po].read() >= 0) {	// granted
			int tmp = sa_grant_in_port[po].read() + 1;
			sa_priority_stage2_new[po].write(tmp % N_ROUTER_PORTS); // modulo of num of input ports
		} else {
			sa_priority_stage2_new[po].write(sa_priority_stage2_reg[po].read());
		}
	}
}

/*
 * update priorities using iSLIP algorithm
 * (only updated if the having grants in the stage2)
 */
template<int N_VCs>
void SWRTAllocator<N_VCs>::sa_priority_update_iSLIP() {

	// clear
	bool check[N_ROUTER_PORTS];
	for (int pi = 0; pi < N_ROUTER_PORTS; pi++) {
		check[pi] = 0;
	}

	for (int po = 0; po < N_ROUTER_PORTS; po++) {
		if (sa_grant_in_port[po].read() >= 0) {	// granted
			int pi = sa_grant_in_port[po].read();// which input port has been granted

			// update priority for this output arbiter
			sa_priority_stage2_new[po].write((pi + 1) % N_ROUTER_PORTS); // modulo of num of input ports

			// update priority for the corresponding input port granted
			int vi = sa_grant_stage1[pi].read();
			sa_priority_stage1_new[pi].write((vi + 1) % N_VCs);	// module of num of VCs

			check[pi] = 1;
		} else {
			// keep priority of this output arbiter
			sa_priority_stage2_new[po].write(sa_priority_stage2_reg[po].read());
		}
	}

	// keep priority of all input arbiters that were not granted in stage2
	for (int pi = 0; pi < N_ROUTER_PORTS; pi++) {
		if (!check[pi]) {
			sa_priority_stage1_new[pi].write(sa_priority_stage1_reg[pi].read());// keep the last priority
		}
	}
}

/*
 * update priorities using winner_take_all algorithm
 * (keep the highest priority for the last winning input VC
 * until its send tail or the corresponding output VC is full)
 */
template<int N_VCs>
void SWRTAllocator<N_VCs>::sa_priority_update_winner_take_all() {

	// update priority for arbiters in the stage1
	for (int pi = 0; pi < N_ROUTER_PORTS; pi++) {
		if (sa_grant_stage1[pi].read() >= 0) {	// granted
			int tmp = sa_grant_stage1[pi].read();
			sa_priority_stage1_new[pi].write(tmp);// keep priority for the input vc granted
		} else {
			int tmp = sa_priority_stage1_reg[pi].read();
			sa_priority_stage1_new[pi].write((tmp + 1) % N_VCs);// prioritize next request
		}
	}

	// update priority for arbiters in the stage2
	for (int po = 0; po < N_ROUTER_PORTS; po++) {
		if (sa_grant_in_port[po].read() >= 0) {	// granted
			int tmp = sa_grant_in_port[po].read();
			sa_priority_stage2_new[po].write(tmp); // keep priority until tail flit sent
		} else {
			int tmp = sa_priority_stage2_reg[po].read();
			sa_priority_stage2_new[po].write((tmp + 1) % N_ROUTER_PORTS); // prioritize next request
		}
	}
}

/*
 * update priorities according to priority of VC:
 * the lowest VC has the highest priority
 */
template<int N_VCs>
void SWRTAllocator<N_VCs>::sa_priority_prio() {

	// update priority for arbiters in the stage1
	for (int pi = 0; pi < N_ROUTER_PORTS; pi++) {
		if (sa_grant_stage1[pi].read() >= 0) {	// granted
			int tmp = sa_grant_stage1[pi].read() + 1;	// update
			tmp = tmp % N_VCs;
			sa_priority_stage1_new[pi].write(tmp);
		} else {
			sa_priority_stage1_new[pi].write(sa_priority_stage1_reg[pi].read());// keep the last priority
		}
	}

	// update priority for arbiters in the stage2
	for (int po = 0; po < N_ROUTER_PORTS; po++) {
		if (sa_grant_in_port[po].read() >= 0) {	// granted
			int tmp = sa_grant_in_port[po].read() + 1;
			sa_priority_stage2_new[po].write(tmp % N_ROUTER_PORTS); // modulo of num of input ports
		} else {
			sa_priority_stage2_new[po].write(sa_priority_stage2_reg[po].read());
		}
	}
}
