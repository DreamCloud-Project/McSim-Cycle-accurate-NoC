/*
 * virtual_router.h
 *
 *  Created on: May 25, 2010
 *      Author: Anh Tran
 *
 *  A virtual_router class from which all router models should be derived
 *
 *	This virtual router defines all I/O ports and virtual empty functions
 *	All real routers have to inherit this virtual router and to implement all these virtual functions
 *
 *  Revised:
 *  	+ Oct 18, 2010:
 *  		- support virtual-channel router with on/off flow control
 *  	+ Nov 18, 2010:
 *  		- support credit-based flow control
 */

#ifndef VIRTUAL_ROUTER_H_
#define VIRTUAL_ROUTER_H_

#include <systemc.h>
#include "../definition.h"
#include "router_activities.h"

//template <unsigned int N_VCs>
class VirtualRouter: public sc_module{
  public:

	sc_in <bool> clk;
	sc_in <bool> reset;


	// input ports
	sc_in <bool> valid_in[ROUTER_PORTS];
	sc_in <Flit> flit_in[ROUTER_PORTS];
//	sc_out <bool> in_full[ROUTER_PORTS][MAX_N_VCs];
	sc_out <bool> in_vc_buffer_rd[ROUTER_PORTS][MAX_N_VCs];	// this VC buffer was read?

	// output ports
	sc_out <bool> valid_out[ROUTER_PORTS];
	sc_out <Flit> flit_out[ROUTER_PORTS];
//	sc_in <bool> out_full[ROUTER_PORTS][MAX_N_VCs];
	sc_in <bool> out_vc_buffer_rd[ROUTER_PORTS][MAX_N_VCs];	// VC buffer of downstream router was read?

	// functions
	// initialize all constants inside the router, such as router location
	virtual void initialize(int x, int y)=0;

	virtual RouterActivities *getRouterActivities() = 0;

	// constructor
	VirtualRouter (sc_module_name name): sc_module(name){}

};

#endif /* VIRTUAL_ROUTER_H_ */
