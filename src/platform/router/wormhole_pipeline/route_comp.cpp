/*
 * route_comp.cpp
 *
 *  Created on: Nov 12, 2010
 *      Author: Anh Tran
 */

#include "route_comp.h"

//========= functions implementation
/*
 * inititialize
 */
void RouteComp::initialize(int x, int y) {
	local_x = x;
	local_y = y;
}

/*
 * getRoutingActivities
 */
RoutingActivities *RouteComp::getRoutingActivities() {
	RoutingActivities *routing_activities = new RoutingActivities();

	routing_activities->n_head_cycles = n_head_cycles;
	routing_activities->n_head_stall_cycles = n_head_stall_cycles;
	routing_activities->n_body_cycles = n_body_cycles;
	routing_activities->n_inactive_cycles = n_inactive_cycles;
	routing_activities->n_output_change_cycles = n_output_change_cycles;

	return routing_activities;
}

/*
 * update activity process
 */
void RouteComp::activity_process() {
	if (reset.read()) {
		last_in_port_state = IDLE;
		last_buffer_out = Flit();

		n_head_cycles = 0;
		n_head_stall_cycles = 0;
		n_body_cycles = 0;
		n_inactive_cycles = 0;

		n_output_change_cycles = 0;
		last_out_port = -8888;
	} else { // clk.pos
		int current_in_port_state = input_port_state.read();
		Flit current_buffer_out = buffer_out.read();

		int current_out_port = out_port.read();

		// update activities
		int current_time = (int) (sc_time_stamp().to_double() / 1000);
		if (current_time >= CommonParameter::warmup_time) {
			if ((current_buffer_out == last_buffer_out)
					&& (current_in_port_state == last_in_port_state)) {
				if (current_in_port_state == WAIT_FOR_GRANT)
					n_head_stall_cycles += 1;
//				|| (current_in_port_state == IDLE))
				else
					n_inactive_cycles += 1;
			} else if (current_in_port_state == WAIT_FOR_GRANT)
				n_head_cycles += 1;
			else {
				n_body_cycles += 1;
			}

			if (current_out_port != last_out_port)
				n_output_change_cycles += 1;
		}

		// update last values
		last_in_port_state = current_in_port_state;
		last_buffer_out = current_buffer_out;

		last_out_port = current_out_port;
	}
}

void RouteComp::route_computation_process() {

	int out_state[N_ROUTER_PORTS];
	int out_credit[N_ROUTER_PORTS];

	if (input_port_state.read() == WAIT_FOR_GRANT) {	// head flit

		Flit flit_tmp = buffer_out.read();
		for (int po = 0; po < N_ROUTER_PORTS; po++) {
			out_state[po] = out_port_state[po].read();
			out_credit[po] = out_credit_remain[po].read();
		}
		out_port.write(
				output_port_cal(flit_tmp.src_x, flit_tmp.src_y, flit_tmp.dst_x,
						flit_tmp.dst_y, flit_tmp.packet_length, out_state,
						out_credit));

	} else {
		out_port.write(-8888);	// dont care
	}
}

int RouteComp::output_port_cal(int src_x, int src_y, int dst_x, int dst_y,
		int packet_length, int out_state[N_ROUTER_PORTS],
		int out_credit[N_ROUTER_PORTS]) {
	int out = routing(src_x, src_y, local_x, local_y, dst_x, dst_y, out_state,
			out_credit, packet_length);
	return out;
}

