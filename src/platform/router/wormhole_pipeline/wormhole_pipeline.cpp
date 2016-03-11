/*
 * one_entry_buffer_router.cpp
 *
 *  Created on: Jan 11, 2011
 *      Author: anhttran
 */

#include "wormhole_pipeline.h"

/*
 * initialize
 */
void WormholePipeline::initialize(int x, int y) {
	local_x = x;
	local_y = y;

	// setup local location for route_comp modules
	for (int pi = 0; pi < N_ROUTER_PORTS; pi++) {
		route_comp[pi]->initialize(x, y);
	}
}

/*
 * getRouterActivities
 */
RouterActivities *WormholePipeline::getRouterActivities() {
	RouterActivities *router_activities = new RouterActivities();

	for (int pi = 0; pi < N_ROUTER_PORTS; pi++) {
		// buffer...
		router_activities->buffer_activities[pi] =
				this->in_buffer[pi]->getBufferActivities();

		// routing...
		router_activities->routing_activities[pi] =
				this->route_comp[pi]->getRoutingActivities();
	}

	// crossbar...
	router_activities->crossbar_activities =
			this->crossbar->getCrossbarActivities();

	for (int po = 0; po < N_ROUTER_PORTS; po++) {
		// arbiters...
		router_activities->arbiter_activities[po] =
				this->sa_allocator->sa_arbiter[po]->get_arbiter_activities();

		// out_credit...
		router_activities->out_credit_counter_activities[po] =
				this->out_credit_remain_update[po]->get_out_credit_counter_activities();
	}

	return router_activities;
}

/*
 * in_port_state_reg
 */
void WormholePipeline::in_port_state_reg_process() {
	if (reset.read()) {
		for (int pi = 0; pi < N_ROUTER_PORTS; pi++) {
			in_port_state_reg[pi].write(IDLE);
		}
	} else {	// clk edge
		for (int pi = 0; pi < N_ROUTER_PORTS; pi++) {
			in_port_state_reg[pi].write(in_port_state[pi].read());
		}
	}
}

/*
 * sa_grant_reg
 */
void WormholePipeline::sa_grant_reg_process() {
	if (reset.read()) {
		for (int po = 0; po < N_ROUTER_PORTS; po++) {
			sa_grant_reg[po].write(NOT_GRANTED);
		}
	} else {
		for (int po = 0; po < N_ROUTER_PORTS; po++) {
			sa_grant_reg[po].write(sa_grant[po].read());
		}
	}
}

/*
 * out_port_state_reg
 */
void WormholePipeline::out_port_state_reg_process() {
	if (reset.read()) {
		for (int po = 0; po < N_ROUTER_PORTS; po++) {
			out_port_state_reg[po].write(IDLE);
		}
	} else {	// clk edge
		for (int po = 0; po < N_ROUTER_PORTS; po++) {
			out_port_state_reg[po].write(out_port_state[po].read());
		}
	}
}

/*
 * sa_priority_reg
 */
void WormholePipeline::sa_priority_reg_process() {
	if (reset.read()) {
		for (int po = 0; po < N_ROUTER_PORTS; po++) {
			sa_priority_reg[po].write(0);
		}
	} else {
		for (int po = 0; po < N_ROUTER_PORTS; po++) {
			sa_priority_reg[po].write(sa_priority[po].read());
		}
	}
}

/*
 * sa_grant_to_allocated
 */
void WormholePipeline::sa_grant_to_allocated_process() {
	for (int pi = 0; pi < N_ROUTER_PORTS; pi++) {	// input ports
		bool check = 0;
		for (int po = 0; po < N_ROUTER_PORTS; po++) {	// output ports
			if (sa_grant[po].read() == pi) {
				sa_allocated[pi].write(po);
				check = 1;
				break;
			}
		}

		if (!check) {
			sa_allocated[pi].write(NOT_ALLOCATED);
		}
	}
}

/*
 * in_port_rd_en
 */
void WormholePipeline::in_port_rd_process() {
	for (int pi = 0; pi < N_ROUTER_PORTS; pi++) {
		if ((sa_allocated[pi].read() >= 0) && !buffer_empty[pi].read()) {// allocated to an output port
			in_vc_buffer_rd_pp[pi][0][0].write(true);
		} else
			in_vc_buffer_rd_pp[pi][0][0].write(false);
	}
}

/*
 * sa_allocated_reg
 */
void WormholePipeline::sa_allocated_reg_process() {
	if (reset.read()) {
		for (int pi = 0; pi < N_ROUTER_PORTS; pi++) {
			sa_allocated_reg[pi].write(NOT_ALLOCATED);
		}
	} else {
		for (int pi = 0; pi < N_ROUTER_PORTS; pi++) {
			sa_allocated_reg[pi].write(sa_allocated[pi].read());
		}
	}
}

/*
 * crossbar_in_valid
 */
void WormholePipeline::crossbar_in_valid_process() {
	for (int pi = 0; pi < N_ROUTER_PORTS; pi++) {
		crossbar_in_valid[pi].write(!buffer_empty[pi].read());
	}
}

/*
 * credit_plus
 */
void WormholePipeline::credit_plus_process() {
	if (reset.read()) {
		for (int po = 0; po < N_ROUTER_PORTS; po++)
			credit_plus[po].write(0);
	} else {
		for (int po = 0; po < N_ROUTER_PORTS; po++)
			credit_plus[po].write(out_vc_buffer_rd[po][0]);
	}
}

/*
 * credit_minus
 */
void WormholePipeline::credit_minus_process() {
	if (reset.read()) {
		for (int po = 0; po < N_ROUTER_PORTS; po++) {
			credit_minus[po].write(0);
		}
	} else {
		for (int po = 0; po < N_ROUTER_PORTS; po++) {
			credit_minus[po].write(valid_out_pp[po][0].read());
		}
	}
}

/*
 * out_port_ready_reg
 */
void WormholePipeline::out_credit_remain_reg_process() {
	if (reset.read()) {
		for (int po = 0; po < N_ROUTER_PORTS; po++) {
//			out_credit_remain_reg[po].write(1);
			out_credit_remain_reg[po].write((int) RouterParameter::buffer_size);
		}
	} else {
		for (int po = 0; po < N_ROUTER_PORTS; po++) {
			out_credit_remain_reg[po].write(out_credit_remain[po].read());
		}
	}
}

/*
 * tail_out_reg
 */
void WormholePipeline::tail_out_reg_process() {
	if (reset.read()) {
		for (int po = 0; po < N_ROUTER_PORTS; po++) {
			tail_out_reg[po].write(0);
		}
	} else {
		for (int po = 0; po < N_ROUTER_PORTS; po++) {
			if (valid_out_pp[po][0].read() && flit_out_pp[po][0].read().tail)
				tail_out_reg[po].write(1);
			else
				tail_out_reg[po].write(0);
		}
	}
}

/*
 * one pipeline simulation
 */
void WormholePipeline::one_pipeline_process() {
	int vc = 0;
	for (int pi = 0; pi < N_ROUTER_PORTS; pi++) {
		in_vc_buffer_rd[pi][vc].write(in_vc_buffer_rd_pp[pi][vc][0].read());
		int po = pi;
		valid_out[po].write(valid_out_pp[po][0].read());
		flit_out[po].write(flit_out_pp[po][0].read());
	}
}

/*
 * multi-pipeline stages simulation
 */
void WormholePipeline::multi_pipeline_process() {
	int vc = 0;
	if (reset.read()) {
		for (int po = 0; po < N_ROUTER_PORTS; po++) {
			int pi = po;

			for (int l = 1; l < RouterParameter::n_pipeline_stages - 1; l++) {
				valid_out_pp[po][l].write(0);
				flit_out_pp[po][l].write(Flit());
				in_vc_buffer_rd_pp[pi][vc][l].write(false);
			}

			valid_out[po].write(0);
			flit_out[po].write(Flit());

			in_vc_buffer_rd[pi][vc].write(0);
		}
	} else {
		for (int po = 0; po < N_ROUTER_PORTS; po++) {
			int pi = po;

			// Internal flit and valid signals between pipeline stages
			int n_out_regs = RouterParameter::n_pipeline_stages - 1;
			for (int l = 1; l < n_out_regs; l++) {
				valid_out_pp[po][l].write(valid_out_pp[po][l - 1].read());
				flit_out_pp[po][l].write(flit_out_pp[po][l - 1].read());
			}

			// Router flit and valid output signals are the ones of the last pipeline stage
			valid_out[po].write(valid_out_pp[po][n_out_regs - 1].read());
			flit_out[po].write(flit_out_pp[po][n_out_regs - 1].read());

			// Get something depending on the number of pipeline stages
			int n_in_buffer_rd_regs;
			switch (RouterParameter::n_pipeline_stages) {
			case 2:
				n_in_buffer_rd_regs = 1;
				break;
			case 3:
				n_in_buffer_rd_regs = 2;
				break;
			case 4:
				n_in_buffer_rd_regs = 2;
				break;
			case 5:
				n_in_buffer_rd_regs = 3;
				break;
			default:
				cout << "This number of router pipeline stages is not supported"
						<< endl;
				exit(0);
			}

			// Internal in_buffer signals between pipeline stages
			for (int l = 1; l < n_in_buffer_rd_regs; l++) {
				in_vc_buffer_rd_pp[pi][vc][l].write(
						in_vc_buffer_rd_pp[pi][vc][l - 1].read());
			}

			// Router in_vc_buffer_rd output signals is the one of the last pipeline stage
			// This signal indicates that the flit on the input port has been read by the router
			in_vc_buffer_rd[pi][vc].write(
					in_vc_buffer_rd_pp[pi][vc][n_in_buffer_rd_regs - 1].read());
		}
	}
}
