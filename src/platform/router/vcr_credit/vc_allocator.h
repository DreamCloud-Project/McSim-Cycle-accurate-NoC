/*
 * vc_allocator.h
 *
 *  Created on: Nov 15, 2010
 *      Author: Anh Tran
 *
 *  New behavior model for fast simulation
 */

#ifndef VC_ALLOCATOR_H_
#define VC_ALLOCATOR_H_

#include <systemc.h>

#include "support_modules/vca_stage1.h"
#include "support_modules/vca_stage2.h"

using namespace std;

template <int N_VCs>
SC_MODULE (VCAllocator){
	sc_in <int> out_port_req_reg[N_ROUTER_PORTS*N_VCs];
	sc_in <bool> tail_reg[N_ROUTER_PORTS*N_VCs];
	sc_in <int> in_vc_state_reg[N_ROUTER_PORTS*N_VCs];
	sc_in <int> out_vc_state_reg[N_ROUTER_PORTS*N_VCs];
	sc_in <int> vca_priority_stage1_reg[N_ROUTER_PORTS*N_VCs];
	sc_in <int> vca_priority_stage2_reg[N_ROUTER_PORTS*N_VCs];

	// vca_grant[indexo] = indexi, means ouput vc[indexo]=[po][vo] is reserved for input vc [indexi]=[pi][vi]
	sc_out <int> vca_grant[N_ROUTER_PORTS*N_VCs];
	// update priority for arbiter of stage1
	sc_out <int> vca_priority_stage1_new[N_ROUTER_PORTS*N_VCs];
	// update priority for arbiter of stage2
	sc_out <int> vca_priority_stage2_new[N_ROUTER_PORTS*N_VCs];

	//========== modules
	// each arbiter for one input VC, choose an output vc
	VCAStage1<N_VCs> *arbiter_stage1[N_ROUTER_PORTS*N_VCs];

	// each arbiter for one output VC, choose an input VC while winners of stage1
	VCAStage2<N_VCs> *arbiter_stage2[N_ROUTER_PORTS*N_VCs];


	//========== signals
	sc_signal <int> vca_grant_stage1[N_ROUTER_PORTS*N_VCs];


	//========= processes
	void vca_priority_new_process();

	//========== constructor
	SC_CTOR (VCAllocator){
		// stage1: P*V arbiters, each for an input VC
		string arbiter_stage1_name;
		for (int indexi=0; indexi<N_ROUTER_PORTS*N_VCs; indexi++){
			arbiter_stage1_name = "VCA_Arbiter_Stage1[" + int_to_str(indexi) + "]";
			arbiter_stage1[indexi] = new VCAStage1<N_VCs>(arbiter_stage1_name.data());

			arbiter_stage1[indexi]->in_vc_index = indexi;
			arbiter_stage1[indexi]->out_port_req_reg(out_port_req_reg[indexi]);
			arbiter_stage1[indexi]->tail_reg(tail_reg[indexi]);
			arbiter_stage1[indexi]->in_vc_state_reg(in_vc_state_reg[indexi]);
			arbiter_stage1[indexi]->vca_priority_stage1_reg(vca_priority_stage1_reg[indexi]);
			for (int indexo=0; indexo<N_ROUTER_PORTS*N_VCs; indexo++){
				arbiter_stage1[indexi]->out_vc_state_reg[indexo](out_vc_state_reg[indexo]);
			}
			arbiter_stage1[indexi]->vca_grant_stage1(vca_grant_stage1[indexi]);
		}

		// stage2: P*V arbiters, each for one output VC
		string arbiter_stage2_name;
		for (int indexo=0; indexo<N_ROUTER_PORTS*N_VCs; indexo++){
			arbiter_stage2_name = "VCA_Arbiter_Stage2[" + int_to_str(indexo) + "]";
			arbiter_stage2[indexo] = new VCAStage2<N_VCs>(arbiter_stage2_name.data());

			arbiter_stage2[indexo]->out_vc_index = indexo;
			for (int indexi=0; indexi<N_ROUTER_PORTS*N_VCs; indexi++){
				arbiter_stage2[indexo]->vca_grant_stage1[indexi](vca_grant_stage1[indexi]);
			}
			arbiter_stage2[indexo]->vca_priority_stage2_reg(vca_priority_stage2_reg[indexo]);
			arbiter_stage2[indexo]->vca_grant_stage2(vca_grant[indexo]);
		}

		// update priorities
		SC_METHOD (vca_priority_new_process);
		for (int indexi=0; indexi<N_ROUTER_PORTS*N_VCs; indexi++){
			sensitive << vca_priority_stage1_reg[indexi];
			sensitive << vca_grant_stage1[indexi];
		}
		for (int indexo=0; indexo<N_ROUTER_PORTS*N_VCs; indexo++){
			sensitive << vca_priority_stage2_reg[indexo];
			sensitive << vca_grant[indexo];
		}

	}

  private:
	void vca_priority_update_roundrobin();
	void vca_priority_update_iSLIP();
};

#endif /* VC_ALLOCATOR_H_ */

//=============================================================
//========================= function implementation

/*
 * update priority of arbiters in the allocator
 * choose among algorithms: round-robin, iSLIP, etc...
 */
template <int N_VCs>
void VCAllocator<N_VCs>::vca_priority_new_process(){
	switch (RouterParameter::vca_type){
		case (VCA_ROUNDROBIN):
				vca_priority_update_roundrobin();
			break;
		case (VCA_iSLIP):
				vca_priority_update_iSLIP();
			break;
		default:;
	}
}

/*
 * update priorities using round-robin algorithm
 */
template <int N_VCs>
void VCAllocator<N_VCs>::vca_priority_update_roundrobin(){
	// update priority for arbiters in the stage1
	for (int indexi=0; indexi<N_ROUTER_PORTS*N_VCs; indexi++){
		if (vca_grant_stage1[indexi].read() >= 0){	// granted
			int tmp = vca_grant_stage1[indexi].read() + 1;	// update
			vca_priority_stage1_new[indexi].write(tmp % (N_ROUTER_PORTS*N_VCs)); // modulo of num of all VCs
		}
		else{
			vca_priority_stage1_new[indexi].write(vca_priority_stage1_reg[indexi].read());	// keep the last priority
		}
	}

	// update priority for arbiters in the stage2
	for (int indexo=0; indexo<N_ROUTER_PORTS*N_VCs; indexo++){
		if (vca_grant[indexo].read() >= 0){	// granted
			int tmp = vca_grant[indexo].read() + 1;
			vca_priority_stage2_new[indexo].write(tmp % (N_ROUTER_PORTS*N_VCs));	// modulo of num of all VCs
		}
		else{
			vca_priority_stage2_new[indexo].write(vca_priority_stage2_reg[indexo].read());
		}
	}
}

/*
 * update priorities using iSLIP algorithm
 * (only updated if the final vca_grants change)
 */
template <int N_VCs>
void VCAllocator<N_VCs>::vca_priority_update_iSLIP(){
	bool check[N_ROUTER_PORTS*N_VCs];
	//clear
	for (int indexi=0; indexi < N_ROUTER_PORTS*N_VCs; indexi++){
		check[indexi] = 0;
	}

	for (int indexo=0; indexo<N_ROUTER_PORTS*N_VCs; indexo++){
		if (vca_grant[indexo].read() >= 0){	// granted
			int indexi = vca_grant[indexo].read();
			vca_priority_stage2_new[indexo].write((indexi+1) % (N_ROUTER_PORTS*N_VCs));	// modulo of num of all VCs

			// update priority for the input vc[indexi]
			int granti = vca_grant_stage1[indexi].read();
			vca_priority_stage1_new[indexi].write((granti+1) % (N_ROUTER_PORTS*N_VCs));

			check[indexi] = 1;
		}
		else{
			vca_priority_stage2_new[indexo].write(vca_priority_stage2_reg[indexo].read());
		}
	}

	for (int indexi=0; indexi < N_ROUTER_PORTS*N_VCs; indexi++){
		if (!check[indexi])
			vca_priority_stage1_new[indexi].write(vca_priority_stage1_reg[indexi].read());	// keep the last priority
	}

}
