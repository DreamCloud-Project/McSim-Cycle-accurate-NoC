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

	// Input interface
	sc_in <bool> valid_in;
	sc_in <Flit> flit_in;

	// Signal from virtual channels of the local router port saying that one buffer entry is available
	sc_in <bool> out_vc_buffer_rd[MAX_N_VCS];

	// output interface
	sc_out <bool> valid_out;
	sc_out <Flit> flit_out;

	sc_out <bool> in_vc_buffer_rd[MAX_N_VCS];


	// initialize all constants inside the processor (x,y)
	virtual void initialize(int x, int y)=0;

	// evaluation
	virtual ProcEvaluationFactors *evaluation()=0;

	// constructor
	VirtualProc (sc_module_name name): sc_module(name){}

};

#endif /* VIRTUAL_PROC_H_ */
