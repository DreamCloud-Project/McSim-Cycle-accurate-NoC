/*
 * sa_stage1.h
 *
 *  Created on: Nov 15, 2010
 *      Author: Anh Tran
 *
 *  A arbiter at stage1 of the Switch Allocator
 *  Behavior model for fast simulation
 */

#ifndef SA_STAGE1_H_
#define SA_STAGE1_H_

#include <systemc.h>
#include "../../router_parameters.h"

template <int N_VCs>
SC_MODULE (SAStage1){
	int in_port;
	sc_in <bool> sa_req_adjusted[N_VCs];
	sc_in <int> sa_priority_stage1_reg;

	sc_out <int> sa_grant_stage1;	// grant for an input VC of input port [pi]

	void sa_stage1_process();

	SC_CTOR (SAStage1){
		SC_METHOD (sa_stage1_process);
		for (int vi=0; vi<N_VCs; vi++){
			sensitive << sa_req_adjusted[vi];
		}
		sensitive << sa_priority_stage1_reg;
	}
};

#endif /* SA_STAGE1_H_ */

//=========================================
//=============== function implementation
/*
 * choose an input VC among many requests
 */
template <int N_VCs>
void SAStage1<N_VCs>::sa_stage1_process(){
	int check = -1;
	bool done = 0;

	int vi=0;
	while ((!done) && (vi<N_VCs)){	// consider all input VCs of this input port
		if (sa_req_adjusted[vi].read()){	// if input [vi] requests SA
			if (vi >= sa_priority_stage1_reg.read()){	// consider priority
				sa_grant_stage1.write(vi);	// grant vi to this input port

				done = 1;
			}
			else {
				if (check == -1) check = vi;
			}
		}
		else {
		}

		vi = vi + 1;
	}

	if (!done){
		if (check != -1){	// existing an grant1 to this output vc
			sa_grant_stage1.write(check);
		}
		else {	// there is no request
			sa_grant_stage1.write(NOT_GRANTED);
		}
	}

}
