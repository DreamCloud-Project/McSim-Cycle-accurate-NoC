/*
 * route_comp.h
 *
 *  Created on: Nov 12, 2010
 *      Author: Anh Tran
 *
 * Output port computation for a input packet
 */

#ifndef ROUTE_COMP_H_
#define ROUTE_COMP_H_

#include "systemc.h"
#include "../router_parameters.h"
#include "../routing_algorithms.h"
#include "../../definition.h"
#include "../router_activities.h"

SC_MODULE (RouteComp) {
	int local_x;
	int local_y;

	sc_in<bool> reset;
	sc_in<bool> clk;

	sc_in<int> input_port_state;
	sc_in<Flit> buffer_out;

	sc_in<int> out_port_state[N_ROUTER_PORTS];	// for adaptive routing
	sc_in<int> out_credit_remain[N_ROUTER_PORTS];	// for adaptive routing

	sc_out<int> out_port;

	void route_computation_process();

	// for energy estimation
	void activity_process();

	RoutingActivities *getRoutingActivities();

	SC_CTOR (RouteComp) {
		SC_METHOD(route_computation_process);
		sensitive << input_port_state << buffer_out; // << valid_check;
		for (int po = 0; po < N_ROUTER_PORTS; po++) {
			sensitive << out_port_state[po];
			sensitive << out_credit_remain[po];
		}

		SC_METHOD(activity_process);
		sensitive << reset.pos() << clk.pos();
	}

	void initialize(int x, int y);

	int output_port_cal(int src_x, int src_y, int dst_x, int dst_y,
			int packet_length, int out_state[N_ROUTER_PORTS],
			int out_credit[N_ROUTER_PORTS]);

	// hack to set route back
	void set_route_back(Flit flit);

private:

	// for energy estimation
	int n_head_cycles;
	int n_head_stall_cycles;
	int n_body_cycles;
	int n_inactive_cycles;

	int n_output_change_cycles;

	int last_in_port_state;
	Flit last_buffer_out;

	int last_out_port;
};

#endif /* ROUTE_COMP_H_ */

