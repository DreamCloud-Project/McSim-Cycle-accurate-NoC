/*
 * vca_stage2.h
 *
 *  Created on: Nov 12, 2010
 *      Author: Anh Tran
 *
 *  an Arbiter in the stage2 of VC allocator
 */

#ifndef VCA_STAGE2_H_
#define VCA_STAGE2_H_

#include <systemc.h>
#include "../../router_parameters.h"

template <int N_VCs>
SC_MODULE (VCAStage2){
	int out_vc_index;	// the global index of considered output vc

	sc_in <int> vca_grant_stage1[N_ROUTER_PORTS*N_VCs];
	sc_in <int> vca_priority_stage2_reg;

	sc_out <int> vca_grant_stage2;

	void vca_stage2_process();

	SC_CTOR (VCAStage2){
		SC_METHOD (vca_stage2_process);
		for (int indexo=0; indexo<N_ROUTER_PORTS*N_VCs; indexo++){
			sensitive << vca_grant_stage1[indexo];
		}
		sensitive << vca_priority_stage2_reg;
	}
};

#endif /* VCA_STAGE2_H_ */

template <int N_VCs>
void VCAStage2<N_VCs>::vca_stage2_process(){
	int check = -1;
	bool done = 0;

	int indexi=0;
	while ((!done) && (indexi<N_ROUTER_PORTS*N_VCs)){	// consider all grants from stage1
		if (vca_grant_stage1[indexi].read() == out_vc_index){	// consider only grant1s to the same this output vc
			if (indexi >= vca_priority_stage2_reg.read()){	// consider priority
				vca_grant_stage2.write(indexi);	// grant output vc[vo] to this input VC

				done = 1;
			}
			else {
				if (check == -1) check = indexi;
			}
		}
		else {
		}

		indexi = indexi + 1;
	}

	if (!done){
		if (check != -1){	// existing an grant1 to this output vc
			vca_grant_stage2.write(check);
		}
		else {	// there is no request
			vca_grant_stage2.write(NOT_GRANTED);
		}
	}

}
