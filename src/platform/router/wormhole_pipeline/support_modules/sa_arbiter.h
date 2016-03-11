/*
 * sa_arbiter.h
 *
 *  Created on: Nov 23, 2010
 *      Author: Anh Tran
 *
 *  An arbiter for output port [po]
 *
 */

#ifndef SA_ARBITER_H_
#define SA_ARBITER_H_

#include <systemc.h>
#include "../../router_parameters.h"
#include "../../router_activities.h"
#include "../../../common_parameters.h"

SC_MODULE (SA_Arbiter){
	int out_port;

	sc_in <bool> clk;
	sc_in <bool> reset;

	sc_in <int> sa_req_adjusted[N_ROUTER_PORTS];

	sc_in <int> sa_priority_reg;
	sc_in <int> out_port_state;

	sc_in <int> out_credit_remain;

	sc_out <int> sa_grant;

	void sa_arbiter_process();

	// for power estimation
	void arbiter_activity_process();

	ArbiterActivities *get_arbiter_activities();

	SC_CTOR (SA_Arbiter){
		SC_METHOD (sa_arbiter_process);
		for (int pi=0; pi<N_ROUTER_PORTS; pi++){
			sensitive << sa_req_adjusted[pi];
		}
		sensitive << sa_priority_reg << out_port_state;
		sensitive << out_credit_remain;

		SC_METHOD (arbiter_activity_process);
		sensitive << reset.pos() << clk.pos();
	}

private:
	// for power estimation
	bool last_req[N_ROUTER_PORTS];
	bool last_hold;
	bool last_out_port_ready;

	int n_active_cycles;
	int n_hold_cycles;
	int n_not_ready_cycles;
	int n_inactive_cycles;

	int n_output_change_cycles;
	int last_sa_grant;
};

#endif /* SA_ARBITER_H_ */

//============================================
