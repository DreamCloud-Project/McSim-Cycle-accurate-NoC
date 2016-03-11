/*
 * route_comp.cpp
 *
 *  Created on: Nov 12, 2010
 *      Author: Anh Tran
 */

#include "route_comp_vc.h"

//========= functions implementation
void RouteCompVC::route_computation_process() {
	if (in_vc_state.read() == IDLE)
		out_port.write(NOT_REQ);
	else {
		Flit flit_tmp = buffer_out.read();
		if (!buffer_empty.read() && flit_tmp.head) {// only allow to compute if flit is head
			out_port.write(
					output_port_cal(flit_tmp, RouterParameter::routing_type));
		}
	}
}

int RouteCompVC::output_port_cal(Flit flit, int type) {
	int out_state[N_ROUTER_PORTS];
	int out_credit[N_ROUTER_PORTS];
	int out = routing(flit.src_x, flit.src_y, local_x, local_y, flit.dst_x,
			flit.dst_y, out_state,
			out_credit, flit.packet_length);
	return out;
}

