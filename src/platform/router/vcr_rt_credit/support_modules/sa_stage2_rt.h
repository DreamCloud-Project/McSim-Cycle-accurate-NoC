/*
 * sa_stage2.h
 *
 *  Created on: Nov 15, 2010
 *      Author: Anh Tran
 *
 *  an arbiter at stage2 of SA
 *
 *  Revised:
 *  	+ 2010.11.18:
 *  		- output grant now has two separate signals (to avoid a division later on):
 *  			1) show input port is granted
 *  			2) show input vc of that input port is granted
 */

#ifndef SA_STAGE2_RT_H_
#define SA_STAGE2_RT_H_

#include <systemc.h>
#include "../../router_parameters.h"
#include "../../../MemoryProfiled.h"

#ifdef MEM_PROF
template<int N_VCs>
class SAStage2RT: public sc_module, MemoryProfiled<SAStage2RT<N_VCs> > {
#else
template<int N_VCs>
class SAStage2RT: public sc_module {
#endif

public:

//template<int N_VCs>
//SC_MODULE (SAStage2RT) {
	int out_port;

	sc_in<int> sa_grant_stage1[N_ROUTER_PORTS];
	sc_in<int> out_port_req_reg[N_ROUTER_PORTS][N_VCs];
	sc_in<int> sa_priority_stage2_reg;

	sc_out<int> sa_grant_stage2_in_port;	// = pi
	sc_out<int> sa_grant_stage2_in_vc;		// = vi

	void sa_stage2_process();

	SC_CTOR (SAStage2RT) {
		SC_METHOD(sa_stage2_process);
		for (int pi = 0; pi < N_ROUTER_PORTS; pi++) {
			sensitive << sa_grant_stage1[pi];
			for (int vi = 0; vi < N_VCs; vi++) {
				sensitive << out_port_req_reg[pi][vi];
			}
		}
		sensitive << sa_priority_stage2_reg;
	}
};

#endif /* SA_STAGE2_H_ */

/*
 * choose an input port from many requests
 */
template<int N_VCs>
void SAStage2RT<N_VCs>::sa_stage2_process() {
	int in_port_check = -1;
	int in_vc_check = -1;

	bool done = 0;

	int pi = 0;
	while ((!done) && (pi < N_ROUTER_PORTS)) {	// consider all input ports
		if (sa_grant_stage1[pi].read() >= 0) {// if input port [pi] has request
			int vi = sa_grant_stage1[pi].read();// request is from input vc [vi]
			if (out_port_req_reg[pi][vi].read() == out_port) {// and the request is to this output port
				if (pi >= sa_priority_stage2_reg.read()) {	// consider priority
					sa_grant_stage2_in_port.write(pi);
					sa_grant_stage2_in_vc.write(vi);

					done = 1;
				} else {
					if (in_port_check == -1) {
						in_port_check = pi;
						in_vc_check = vi;
					}
				}
			}
		}
		pi = pi + 1;
	}

	if (!done) {
		if (in_port_check != -1) {	// existing an grant1 to this output vc
			sa_grant_stage2_in_port.write(in_port_check);
			sa_grant_stage2_in_vc.write(in_vc_check);
		} else {	// there is no request
			sa_grant_stage2_in_port.write(NOT_GRANTED);
			sa_grant_stage2_in_vc.write(NOT_GRANTED);
		}
	}

}
