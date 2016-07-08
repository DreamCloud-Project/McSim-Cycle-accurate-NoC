/*
 * in_vc_state_comp.h
 *
 *  Created on: Nov 12, 2010
 *      Author: Anh Tran
 *
 *  Updating state of an input VC
 */

#ifndef IN_VC_STATE_UPDATE_RT_H_
#define IN_VC_STATE_UPDATE_RT_H_

#include "systemc.h"
#include "../../router_parameters.h"
#include "../../../MemoryProfiled.h"

#ifdef MEM_PROF
class InVCStateUpdateRT : public sc_module, MemoryProfiled<InVCStateUpdateRT> {
#else
class InVCStateUpdateRT : public sc_module {
#endif

public:
	int in_port;	// input port identification
	int in_vc;		// input VC of the input port above

//	sc_in <bool> reset;
	sc_in <bool> is_empty;
	sc_in <bool> is_tail_reg;
	sc_in <int> out_vc_allocated;	// = [vo] = which output VC is allocated for this input VC
	sc_in <int> in_vc_state_reg;

	sc_out <int> in_vc_state;

	void in_vc_state_process(){
			switch (in_vc_state_reg.read()){
				case (IDLE):
						if (!is_empty.read())		// having a packet
							in_vc_state.write(WAIT_FOR_GRANT);	// assumed finishing RC
						else
							in_vc_state.write(IDLE);
						break;
				case (WAIT_FOR_GRANT):
						if (out_vc_allocated.read() >= 0)	// is allocated for an out VC
							in_vc_state.write(out_vc_allocated.read());	// record this output VC
						else
							in_vc_state.write(WAIT_FOR_GRANT);
						break;
				default:	// >= 0 that means it is ACTIVE
						if (is_tail_reg.read()){
							if (is_empty.read())	// no more packet
								in_vc_state.write(IDLE);
							else	// having another packet in the input VC
								in_vc_state.write(WAIT_FOR_GRANT);
						}
						else
							in_vc_state.write(in_vc_state_reg.read());

						break;
			}
	}

	SC_HAS_PROCESS(InVCStateUpdateRT);
	InVCStateUpdateRT(sc_module_name name_) : sc_module(name_) {
		SC_METHOD (in_vc_state_process);
		sensitive << is_empty << is_tail_reg << out_vc_allocated << in_vc_state_reg;
	}
};

#endif /* IN_VC_STATE_UPDATE_H_ */
