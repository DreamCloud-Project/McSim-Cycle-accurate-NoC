/*
 * vca_stage2.h
 *
 *  Created on: Nov 12, 2010
 *      Author: Anh Tran
 *
 *  an Arbiter in the stage2 of VC allocator
 */

#ifndef VCA_STAGE2_RT_H_
#define VCA_STAGE2_RT_H_

#include <systemc.h>
#include "../../router_parameters.h"

template<int N_VCs>
SC_MODULE (VCAStage2OutputVCArbitrerRT) {
	int out_vc_index;	// the global index of considered output vc

	sc_in<int> vca_grant_stage1[N_ROUTER_PORTS * N_VCs];
	sc_in<int> vca_priority_stage2_reg;
	sc_out<int> vca_grant_stage2;
	int portId;
	int vcId;

	void vca_stage2_process();

	SC_HAS_PROCESS(VCAStage2OutputVCArbitrerRT);
	VCAStage2OutputVCArbitrerRT(sc_module_name name, int port, int vc) :
			portId(port), vcId(vc) {
		SC_METHOD(vca_stage2_process);
		for (int indexo = 0; indexo < N_ROUTER_PORTS * N_VCs; indexo++) {
			sensitive << vca_grant_stage1[indexo];
		}
		sensitive << vca_priority_stage2_reg;
		dont_initialize();
	}
};

#endif /* VCA_STAGE2_H_ */

template<int N_VCs>
void VCAStage2OutputVCArbitrerRT<N_VCs>::vca_stage2_process() {
	int check = -1;
	bool done = 0;
	for (int inPortId = 0; inPortId < N_ROUTER_PORTS; inPortId++) {
		for (int inVcId = 0; inVcId < N_VCs; inVcId++) {
			int indexi = inPortId * N_VCs + inVcId;
			if (vca_grant_stage1[indexi].read() == out_vc_index) {// consider only grant1s to this output vc
				if (indexi >= vca_priority_stage2_reg.read()) {	// consider priority
					if (inVcId != this->vcId) {
						cerr << "Per priority VC not respected" << endl;
						exit(-1);
					}
					vca_grant_stage2.write(indexi);	// grant output vc[vo] to this input VC
					done = 1;
					break;
				} else {
					if (check == -1) {
						check = indexi;
					}
				}
			}
		}
		if (done) {
			break;
		}
	}

	if (!done) {
		if (check != -1) {	// existing an grant1 to this output vc
			vca_grant_stage2.write(check);
		} else {	// there is no request
			vca_grant_stage2.write(NOT_GRANTED);
		}
	}
}
