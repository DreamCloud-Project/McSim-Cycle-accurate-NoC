/*
 * vca_arbiter_stage1.h
 *
 *  Created on: Nov 12, 2010
 *      Author: Anh Tran
 *
 *  an arbiter in the stage1 of the VC allocator.
 *  This is a behavior model for fast simulation
 */

#ifndef VCA_STAGE1_RT_H_
#define VCA_STAGE1_RT_H_

#include <systemc.h>
#include "../../router_parameters.h"

/**
 * One VCAStage1 for each input VC.
 * So there is N_VCS * N_ROUTER_PORTS VCAStage 1.
 */
template<int N_VCs>
SC_MODULE (VCAStage1OutputVCChooserRT) {

	int in_port_id; // the input port id
	int in_vc_id; // the VC id in the input port
	int in_vc_index; // the global index of an input vc = in_port_id*N_VCS + VC_id
	sc_in<bool> tail_reg;		// not grant if the flit is a tail
	sc_in<int> out_port_req_reg;	// output port request of this input vc
	sc_in<int> in_vc_state_reg;	// state of this input vc
	sc_in<int> vca_priority_stage1_reg;	// priority or token
	sc_in<int> out_vc_state_reg[N_ROUTER_PORTS * N_VCs];// states of all out vc

	sc_out<int> vca_grant_stage1;

	void vca_stage1_process();

	SC_CTOR (VCAStage1OutputVCChooserRT) {
		SC_METHOD(vca_stage1_process);
		sensitive << tail_reg << out_port_req_reg << in_vc_state_reg
				<< vca_priority_stage1_reg;
		dont_initialize();
		for (int po = 0; po < N_ROUTER_PORTS; po++) {	// all output ports
			for (int vo = 0; vo < N_VCs; vo++) {		// all output VC
				int indexo = po * N_VCs + vo;
				sensitive << out_vc_state_reg[indexo];
			}
		}
	}
};

#endif /* VCA_STAGE1_H_ */

template<int N_VCs>
void VCAStage1OutputVCChooserRT<N_VCs>::vca_stage1_process() {
	if ((out_port_req_reg.read() >= 0) && !tail_reg.read()) {// requesting an output port
		int po = out_port_req_reg.read();	// the requested output port
		if (in_vc_state_reg.read() >= 0) {// this input vc is already allocated for an out vc
			int indexo = po * N_VCs + in_vc_state_reg.read();// change to global vc index
			vca_grant_stage1.write(indexo);	// keep this grant until tail flit leaves the input VC
		} else {	// the input VC is not allocated any out VC yet
			bool done = 0;
			int targetIndexo = po * N_VCs + in_vc_id;
			if (out_vc_state_reg[targetIndexo].read() < 0) {// the output vc[vo] is not reserved for any input yet
				vca_grant_stage1.write(targetIndexo);// grant output vc[vo] to this input VC
				done = 1;
//				cerr << "vc chooser rt ask for OVC " << in_vc_id << " on port "
//						<< int2direction(po) << " for IVC " << in_vc_id
//						<< " on port " << int2direction(in_port_id) << endl;
			}
			if (!done) {
				vca_grant_stage1.write(NOT_GRANTED);
			}
		}
	} else {
		vca_grant_stage1.write(NOT_GRANTED);
	}
}
