/*
 * os_allocator.h
 *
 *  Created on: Nov 23, 2010
 *      Author: Anh Tran
 *
 *  Output Switch Allocator
 *
 *  Revised:
 *  	+ Nov 30, 2010: support lookahead routing
 */

#ifndef SA_ALLOCATOR_H_
#define SA_ALLOCATOR_H_

#include "../../common_functions.h"
#include "support_modules/sa_arbiter.h"

using namespace std;

SC_MODULE (SwitchAllocator){
	sc_in <bool> reset;
	sc_in <bool> clk;

	sc_in <int> in_port_state[N_ROUTER_PORTS];
	sc_in <int> in_port_out_port_req[N_ROUTER_PORTS];

	sc_in <int> sa_priority_reg[N_ROUTER_PORTS];
	sc_in <int> out_port_state[N_ROUTER_PORTS];

	sc_in <int> out_credit_remain[N_ROUTER_PORTS];

	sc_out <int> sa_grant[N_ROUTER_PORTS];
	sc_out <int> sa_priority_new[N_ROUTER_PORTS];

	//=========signals
	sc_signal <int> sa_req_adjusted[N_ROUTER_PORTS];

	//========= modules
	SA_Arbiter *sa_arbiter[N_ROUTER_PORTS];	// output arbiters


	//========= processes
	void sa_req_adjusted_process();

	void sa_priority_new_process();

	//========= constructor
	SC_CTOR (SwitchAllocator){
		// adjust req signals
		SC_METHOD (sa_req_adjusted_process);
		for (int pi=0; pi<N_ROUTER_PORTS; pi++){
			sensitive << in_port_state[pi];
			sensitive << in_port_out_port_req[pi];
		}

		// arbiters at stage1
		string arbiter_name;
		for (int po=0; po<N_ROUTER_PORTS; po++){
			arbiter_name = "SA_Arbiter[" + int_to_str(po) +"]";
			sa_arbiter[po] = new SA_Arbiter(arbiter_name.data());

			sa_arbiter[po]->reset(reset);
			sa_arbiter[po]->clk(clk);

			sa_arbiter[po]->out_port = po;
			for (int pi=0; pi<N_ROUTER_PORTS; pi++){
				sa_arbiter[po]->sa_req_adjusted[pi](sa_req_adjusted[pi]);
			}
			sa_arbiter[po]->sa_priority_reg(sa_priority_reg[po]);
			sa_arbiter[po]->out_port_state(out_port_state[po]);
			sa_arbiter[po]->out_credit_remain(out_credit_remain[po]);
			sa_arbiter[po]->sa_grant(sa_grant[po]);
		}

		// update priorities
		SC_METHOD (sa_priority_new_process);
		for (int po=0; po<N_ROUTER_PORTS; po++){
			sensitive << sa_grant[po];
			sensitive << sa_priority_reg[po];
		}

	}
};

#endif /* SA_ALLOCATOR_H_ */

//=====================================
//=========== function implementation
