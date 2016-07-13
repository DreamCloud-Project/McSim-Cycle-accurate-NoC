/*
 * virtual_core.h
 *
 *  Created on: May 2, 2010
 *      Author: Anh Tran
 *
 *  Define of a virtual core with only I/O ports and virtual functions without implemented
 *  All cores have to inherit this virtual core and to implement these virtual functions
 *
 *  Revised:
 *  	+ Oct 25, 2010: now support virtual-channel router
 *  	+ Nov 18, 2010: support credit-based flow control
 */

#ifndef VIRTUAL_PROC_H_
#define VIRTUAL_PROC_H_

#include <systemc.h>
#include "../definition.h"
#include "proc_evaluation.h"

class VirtualProc: public sc_module{
  public:

	// clk and reset
	sc_in <bool> clk;
	sc_in <bool> reset;

	// Signal from the router saying that he received a flit for this proc
	sc_in <bool> valid_in;

	// Signal from the router with the flit to be received by this proc
	sc_in <Flit> flit_in;

	// Signal from virtual channels of the router saying that one buffer entry is available
	sc_in <bool> out_vc_buffer_rd[MAX_N_VCS];

	// Signal to router saying that this proc want to send a flit
	sc_out <bool> valid_out;

	// Signal to router with the flit to be sent by this proc
	sc_out <Flit> flit_out;

	// Signal to router saying saying that this proc has read an input flit on the given VC (the router can free it)
	sc_out <bool> in_vc_buffer_rd[MAX_N_VCS];

	// initialize all constants inside the processor (x,y)
	virtual void initialize(int x, int y)=0;

	// evaluation
	virtual ProcEvaluationFactors *evaluation()=0;

	// constructor
	VirtualProc (sc_module_name name): sc_module(name){}

};

#endif /* VIRTUAL_PROC_H_ */
