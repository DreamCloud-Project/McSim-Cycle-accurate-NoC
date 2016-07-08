/*
 * out_vc_state_update.h
 *
 *  Created on: Nov 15, 2010
 *      Author: Anh Tran
 *
 * Update output VC state
 */

#ifndef OUT_VC_STATE_UPDATE_RT_H_
#define OUT_VC_STATE_UPDATE_RT_H_

#include "systemc.h"
#include "../../router_parameters.h"
#include "../../../MemoryProfiled.h"

#ifdef MEM_PROF
template<int N_VCs>
class OutVCStateUpdateRT: public sc_module, MemoryProfiled<OutVCStateUpdateRT<N_VCs> > {
#else
template<int N_VCs>
class OutVCStateUpdateRT: public sc_module {
#endif

public:

//template <int N_VCs>
//SC_MODULE (OutVCStateUpdateRT){
	int out_port;
	int out_vc;

//	sc_in <bool> reset;
	sc_in <int> vca_grant;
	sc_in <bool> tail_reg[N_ROUTER_PORTS*N_VCs];
	sc_in <int> out_vc_state_reg;

	sc_out <int> out_vc_state;

	void out_vc_state_process(){
//		if (out_port == LOCAL && out_vc>=1){	// not consider these VCs at the LOCAL output port
//			out_vc_state.write(N_ROUTER_PORTS*N_VCs);
//		}
//		else{
			switch (out_vc_state_reg.read()){
				case (IDLE):
						if (vca_grant.read() >= 0){	// the out VC is granted for an input VC
							out_vc_state.write(vca_grant.read());
						}
						else
							out_vc_state.write(IDLE);
					break;
				default: // >=0; this out vc was reserved for an input VC
					int indexi = out_vc_state_reg.read();
					if (tail_reg[indexi].read()){
						out_vc_state.write(IDLE);
					}
					else
						out_vc_state.write(indexi);
					break;
			}
//		}
	}

	SC_CTOR (OutVCStateUpdateRT){
		SC_METHOD (out_vc_state_process);
		sensitive << vca_grant << out_vc_state_reg;
		for (int indexi=0; indexi<N_ROUTER_PORTS*N_VCs; indexi++){
			sensitive << tail_reg[indexi];
		}
	}
};

#endif /* OUT_VC_STATE_UPDATE_H_ */
