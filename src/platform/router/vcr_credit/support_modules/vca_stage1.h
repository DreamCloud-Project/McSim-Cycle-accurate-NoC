/*
 * vca_arbiter_stage1.h
 *
 *  Created on: Nov 12, 2010
 *      Author: Anh Tran
 *
 *  an arbiter in the stage1 of the VC allocator.
 *  This is a behavior model for fast simulation
 */

#ifndef VCA_STAGE1_H_
#define VCA_STAGE1_H_

#include <systemc.h>
#include "../../router_parameters.h"

template <int N_VCs>
SC_MODULE (VCAStage1){
	int in_vc_index;	// the global index of an input vc = in_port_id*V + in_vc_id

	sc_in <bool> tail_reg;		// not grant if the flit is a tail
	sc_in <int> out_port_req_reg;	// output port request of this input vc
	sc_in <int> in_vc_state_reg;	// state of this input vc
	sc_in <int> vca_priority_stage1_reg;	// priority or token
	sc_in <int> out_vc_state_reg[N_ROUTER_PORTS*N_VCs];	// states of all out vc

	sc_out <int> vca_grant_stage1;

	void vca_stage1_process();

	SC_CTOR (VCAStage1){
		SC_METHOD (vca_stage1_process);
		sensitive << tail_reg << out_port_req_reg << in_vc_state_reg << vca_priority_stage1_reg;
		for (int po=0; po<N_ROUTER_PORTS; po++){	// all output ports
			for (int vo=0; vo<N_VCs; vo++){		// all output VC
				int indexo = po*N_VCs + vo;
				sensitive << out_vc_state_reg[indexo];
			}
		}
	}
};

#endif /* VCA_STAGE1_H_ */

template <int N_VCs>
void VCAStage1<N_VCs>::vca_stage1_process(){
	if ((out_port_req_reg.read() >= 0) && (!tail_reg.read())){	// requesting an output port
		int po = out_port_req_reg.read();	// the requested output port
		if (in_vc_state_reg.read() >= 0){	// this input vc is already allocated for an out vc
			int indexo = po*N_VCs + in_vc_state_reg.read();	// change to global vc index
			vca_grant_stage1.write(indexo);	// keep this grant until tail flit leaves the input VC
		}
		else {	// the input VC is not allocated any out VC yet
			int check = -1;
			bool done = 0;

			int vo=0;
			while ((!done) && (vo<N_VCs)){	// consider all out VCs of the output port [po]
				int indexo = po*N_VCs + vo;
				if (out_vc_state_reg[indexo].read() < 0){	// the output vc[vo] is not reserved for any input yet
					if (indexo >= vca_priority_stage1_reg.read()){	// consider priority
					vca_grant_stage1.write(indexo);	// grant output vc[vo] to this input VC

						done = 1;
					}
					else {
						if (check == -1) check = indexo;
					}
				}
				else {
				}

				vo = vo + 1;
			}

			if (!done){
				if (check != -1){	// existing an output vo that is not reserved for any input VC yet
					vca_grant_stage1.write(check);
				}
				else {	// there is no request
					vca_grant_stage1.write(NOT_GRANTED);
				}
			}
		}
	}
	else {
		vca_grant_stage1.write(NOT_GRANTED);
	}
}
