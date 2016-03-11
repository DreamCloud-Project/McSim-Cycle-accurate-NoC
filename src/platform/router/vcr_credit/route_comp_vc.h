/*
 * route_comp.h
 *
 *  Created on: Nov 12, 2010
 *      Author: Anh Tran
 *
 * Output port computation for a input packet
 */

#ifndef ROUTE_COMP_VC_H_
#define ROUTE_COMP_VC_H_

#include "systemc.h"
#include "../router_parameters.h"
#include "../routing_algorithms.h"
#include "../../definition.h"

SC_MODULE (RouteCompVC){
	int local_x;
	int local_y;

	sc_in <int> in_vc_state;
	sc_in <bool> buffer_empty;
	sc_in <Flit> buffer_out;

	sc_out <int> out_port;

	void route_computation_process();

	SC_CTOR (RouteCompVC){
		SC_METHOD (route_computation_process);
		sensitive << in_vc_state << buffer_out << buffer_empty;
	}

  private:
	int output_port_cal(Flit flit, int type);
};

#endif /* ROUTE_COMP_H_ */

