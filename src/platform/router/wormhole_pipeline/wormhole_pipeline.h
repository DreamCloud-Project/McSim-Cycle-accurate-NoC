/*
 * wormhole_pipeline.h
 *
 *  Created on: Nov 22, 2010
 *      Author: Anh Tran
 *
 *  Configurable pipelined wormhole router
 *  	- 1: BF+LRC/SA+ST+LT (not practical, should pipeline the links to avoid power waste on links due to glitches)
 *		- 2: BF+LRC/SA+ST | LT
 *		- 3: BF | LRC/SA+ST | LT
 *		- 4: BF | LRC/SA | ST | LT
 *		- 5: BF | RC | SA | ST | LT
 *		- 6 (not necessary)
 *
 */

#ifndef WORMHOLE_PIPELINE_H_
#define WORMHOLE_PIPELINE_H_

#include <vector>
#include <systemc.h>
#include "../../common_functions.h"
#include "../routing_algorithms.h"
#include "../virtual_router.h"
#include "buffer.h"

#include "support_modules/in_port_state_update.h"
#include "support_modules/out_port_state_update.h"
#include "support_modules/out_credit_remain_update.h"

#include "route_comp.h"
#include "sa_allocator.h"
#include "general_crossbar.h"

using namespace std;

class WormholePipeline: public VirtualRouter{
  public:
	// local address
	int local_x;
	int local_y;

	// initialize constant values
	void initialize(int x, int y);

	// for energy estimation
	RouterActivities *getRouterActivities();

	//========= internal signals
	sc_signal <int> in_port_state[N_ROUTER_PORTS];
	sc_signal <int> in_port_state_reg[N_ROUTER_PORTS];

	sc_signal <int> out_port_state[N_ROUTER_PORTS];
	sc_signal <int> out_port_state_reg[N_ROUTER_PORTS];

	sc_signal <bool> valid_in_reg[N_ROUTER_PORTS];
	sc_signal <Flit> flit_in_reg[N_ROUTER_PORTS];

	sc_signal <bool> buffer_empty[N_ROUTER_PORTS];
	sc_signal <Flit> buffer_out[N_ROUTER_PORTS];

//	sc_signal <bool> valid_check[N_ROUTER_PORTS];	// check whether flit_in_reg is valid
//	sc_signal <bool> valid_check_reg[N_ROUTER_PORTS];

	sc_signal <int> out_port_req[N_ROUTER_PORTS];	// = po

	sc_signal <int> sa_priority[N_ROUTER_PORTS];
	sc_signal <int> sa_priority_reg[N_ROUTER_PORTS];

	sc_signal <int> sa_grant[N_ROUTER_PORTS]; // = pi
	sc_signal <int> sa_grant_reg[N_ROUTER_PORTS]; // = pi

	sc_signal <int> sa_allocated[N_ROUTER_PORTS];	// = po
	sc_signal <int> sa_allocated_reg[N_ROUTER_PORTS];

	sc_signal <bool> crossbar_in_valid[N_ROUTER_PORTS];	// = !buffer_empty

	sc_signal <bool> credit_plus[N_ROUTER_PORTS];
	sc_signal <bool> credit_minus[N_ROUTER_PORTS];

//	sc_signal <bool> out_port_ready[N_ROUTER_PORTS];
//	sc_signal <bool> out_port_ready_reg[N_ROUTER_PORTS];

	sc_signal <int> out_credit_remain[N_ROUTER_PORTS];
	sc_signal <int> out_credit_remain_reg[N_ROUTER_PORTS];

	sc_signal <bool> tail_out_reg[N_ROUTER_PORTS];


	//========= submodules
	Buffer *in_buffer[N_ROUTER_PORTS];

	InPortStateUpdate *in_port_state_update[N_ROUTER_PORTS];

//	ValidCheckUpdate *valid_check_update[N_ROUTER_PORTS];

	RouteComp *route_comp[N_ROUTER_PORTS];

	SwitchAllocator *sa_allocator;

	OutPortStateUpdate *out_port_state_update[N_ROUTER_PORTS];

	GeneralCrossbar<N_ROUTER_PORTS, N_ROUTER_PORTS> *crossbar;

	OutCreditRemainUpdate *out_credit_remain_update[N_ROUTER_PORTS];

	//========= processes
	void in_port_state_reg_process();

//	void input_write_process();

//	void valid_check_reg_process();

	void sa_priority_reg_process();

	void sa_grant_reg_process();

	void out_port_state_reg_process();

	void sa_grant_to_allocated_process();

	void sa_allocated_reg_process();

	void in_port_rd_process();

	void crossbar_in_valid_process();

	void credit_plus_process();

	void credit_minus_process();

//	void out_port_ready_reg_process();
//	void out_credit_remain_process();
	void out_credit_remain_reg_process();

	void tail_out_reg_process();

	void one_pipeline_process();	// 1 pipeline-stage router
	void multi_pipeline_process();	// simulation of pipelining by adding registers at output links

	//========= construtor
	SC_HAS_PROCESS(WormholePipeline);
	WormholePipeline (sc_module_name name): VirtualRouter(name) {
		//============== router traverse
		// input buffer write
		string in_buffer_name;
		for (int pi=0; pi<N_ROUTER_PORTS; pi++){	// all input ports
			in_buffer_name = "in_buffer[" + int_to_str(pi) + "]";
			in_buffer[pi] = new Buffer(in_buffer_name.data());

			in_buffer[pi]->initilize(RouterParameter::buffer_size);

			in_buffer[pi]->clk(clk);
			in_buffer[pi]->reset(reset);

			in_buffer[pi]->valid_in(valid_in[pi]);
			in_buffer[pi]->buffer_in(flit_in[pi]);

			in_buffer[pi]->out_buffer_rd(in_vc_buffer_rd_pp[pi][0][0]);
			in_buffer[pi]->buffer_out(buffer_out[pi]);
			in_buffer[pi]->empty(buffer_empty[pi]);
		}

		// update in_port_state_reg
		SC_METHOD (in_port_state_reg_process);
		sensitive << clk.pos() << reset.pos();

//		// write new input flits
//		SC_METHOD (input_write_process);
//		sensitive << clk.pos() << reset.pos();

		// in port states
		string in_port_state_name;
		for (int pi=0; pi<N_ROUTER_PORTS; pi++){	// all input ports
			in_port_state_name = "in_port_state[" + int_to_str(pi) + "]";
			in_port_state_update[pi] = new InPortStateUpdate(in_port_state_name.data());

			in_port_state_update[pi]->in_port = pi;
			in_port_state_update[pi]->empty(buffer_empty[pi]);
			for (int po=0; po<N_ROUTER_PORTS; po++){
				in_port_state_update[pi]->tail_out_reg[po](tail_out_reg[po]);
			}
			in_port_state_update[pi]->sa_allocated_reg(sa_allocated_reg[pi]);
			in_port_state_update[pi]->input_port_state_reg(in_port_state_reg[pi]);
			in_port_state_update[pi]->input_port_state(in_port_state[pi]);
		}


		// valid_check_reg process
//		SC_METHOD (valid_check_reg_process);
//		sensitive << reset.pos() << clk.pos();

/*
		// valid_check
		string valid_check_name;
		for (int pi=0; pi<N_ROUTER_PORTS; pi++){	// all input ports
			valid_check_name = "valid_check[" + int_to_str(pi) + "]";
			valid_check_update[pi] = new ValidCheckUpdate(valid_check_name.data());

			valid_check_update[pi]->in_port = pi;
			valid_check_update[pi]->valid_in_reg(valid_in_reg[pi]);
			valid_check_update[pi]->sa_allocated_reg(sa_allocated_reg[pi]);
			valid_check_update[pi]->valid_check_reg(valid_check_reg[pi]);
			valid_check_update[pi]->valid_check(valid_check[pi]);
		}
*/

		// output port computation
		string route_comp_name;
		for (int pi=0; pi<N_ROUTER_PORTS; pi++){	// all input ports
			route_comp_name = "route_comp[" + int_to_str(pi) + "]";
			route_comp[pi] = new RouteComp(route_comp_name.data());

			route_comp[pi]->reset(reset);
			route_comp[pi]->clk(clk);

			route_comp[pi]->input_port_state(in_port_state[pi]);
			route_comp[pi]->buffer_out(buffer_out[pi]);
//			route_comp[pi]->valid_check(valid_check[pi]);
			for (int po=0; po<N_ROUTER_PORTS; po++){
				route_comp[pi]->out_port_state[po](out_port_state[po]);
				route_comp[pi]->out_credit_remain[po](out_credit_remain[po]);
			}
			route_comp[pi]->out_port(out_port_req[pi]);

			// hack rt
			//in_buffer[pi]->rc = route_comp[pi];
		}

		// switch allocator
		sa_allocator = new SwitchAllocator("Switch_Allocator");
		sa_allocator->reset(reset);
		sa_allocator->clk(clk);
		for (int pi=0; pi<N_ROUTER_PORTS; pi++){
			sa_allocator->in_port_state[pi](in_port_state[pi]);
			sa_allocator->in_port_out_port_req[pi](out_port_req[pi]);
		}
		for (int po=0; po<N_ROUTER_PORTS; po++){
			sa_allocator->sa_priority_reg[po](sa_priority_reg[po]);
			sa_allocator->out_port_state[po](out_port_state[po]);
			sa_allocator->out_credit_remain[po](out_credit_remain[po]);
			sa_allocator->sa_grant[po](sa_grant[po]);
			sa_allocator->sa_priority_new[po](sa_priority[po]);
		}


		// sa_grant_reg
		SC_METHOD (sa_grant_reg_process);
		sensitive << clk.pos() << reset.pos();

		// out_port_state_reg
		SC_METHOD (out_port_state_reg_process);
		sensitive << clk.pos() << reset.pos();

		// out port states
		string out_port_state_name;
		for (int po=0; po<N_ROUTER_PORTS; po++){	// all input ports
			out_port_state_name = "out_port_state[" + int_to_str(po) + "]";
			out_port_state_update[po] = new OutPortStateUpdate(out_port_state_name.data());

			out_port_state_update[po]->out_port = po;
			out_port_state_update[po]->out_port_state_reg(out_port_state_reg[po]);
			out_port_state_update[po]->sa_grant_reg(sa_grant_reg[po]);
			out_port_state_update[po]->tail_out_reg(tail_out_reg[po]);
			out_port_state_update[po]->out_port_state(out_port_state[po]);
		}

		// update sa_priority_reg
		SC_METHOD (sa_priority_reg_process);
		sensitive << clk.pos() << reset.pos();

		// sa_grant_to_allocated
		SC_METHOD (sa_grant_to_allocated_process);
		for (int po=0; po<N_ROUTER_PORTS; po++){
			sensitive << sa_grant[po];
		}

		// in_port_rd
		SC_METHOD (in_port_rd_process);
		for (int pi=0; pi<N_ROUTER_PORTS; pi++){
			sensitive << sa_allocated[pi] << buffer_empty[pi];
		}

		// sa_allocated_reg
		SC_METHOD (sa_allocated_reg_process);
		sensitive << clk.pos() << reset.pos();

		// crossbar_in_valid
		SC_METHOD (crossbar_in_valid_process);
		for (int pi=0; pi<N_ROUTER_PORTS; pi++){
			sensitive << sa_allocated[pi] << buffer_empty[pi];
		}

		// crossbar
		crossbar = new GeneralCrossbar<N_ROUTER_PORTS, N_ROUTER_PORTS>("Crossbar");
		crossbar->reset(reset);
		crossbar->clk(clk);
		for (int pi=0; pi<N_ROUTER_PORTS; pi++){
			crossbar->crossbar_in[pi](buffer_out[pi]);
			crossbar->crossbar_in_valid[pi](crossbar_in_valid[pi]);
		}
		for (int po=0; po<N_ROUTER_PORTS; po++){
			crossbar->crossbar_grant[po](sa_grant[po]);
			crossbar->crossbar_out[po](flit_out_pp[po][0]);
			crossbar->crossbar_out_valid[po](valid_out_pp[po][0]);
		}

		// credit_plus
		SC_METHOD (credit_plus_process);
		sensitive << reset.pos() << clk.pos();

		// credit_minus
		SC_METHOD (credit_minus_process);
		sensitive << reset.pos() << clk.pos();
//		for (int po=0; po<N_ROUTER_PORTS; po++){
//			sensitive << sa_grant_reg[po];
//		}


		// out_credit_remain
		string out_credit_remain_name;
		for (int po=0; po<N_ROUTER_PORTS; po++){	// all input ports
			out_credit_remain_name = "out_credit_remain[" + int_to_str(po) + "]";
			out_credit_remain_update[po] = new OutCreditRemainUpdate(out_credit_remain_name.data());

			out_credit_remain_update[po]->out_port = po;
			out_credit_remain_update[po]->reset(reset);
			out_credit_remain_update[po]->clk(clk);
			out_credit_remain_update[po]->credit_minus(credit_minus[po]);
			out_credit_remain_update[po]->credit_plus(credit_plus[po]);
			out_credit_remain_update[po]->out_credit_remain_reg(out_credit_remain_reg[po]);
			out_credit_remain_update[po]->out_credit_remain(out_credit_remain[po]);
		}

		// out_port_ready_reg
		SC_METHOD (out_credit_remain_reg_process);
		sensitive << reset.pos() << clk.pos();

		// tail_out_reg
		SC_METHOD (tail_out_reg_process);
		sensitive << reset.pos() << clk.pos();

		// pipeline simulation
		if (RouterParameter::pipeline_type == PIPELINE_TYPE_1){
			SC_METHOD (one_pipeline_process);
			for (int pi=0; pi<N_ROUTER_PORTS; pi++){
				sensitive << in_vc_buffer_rd_pp[pi][0][0];
				sensitive << valid_out_pp[pi][0];
				sensitive << flit_out_pp[pi][0];
			}
		}
		else {	// >= 2
			SC_METHOD (multi_pipeline_process);
			sensitive << reset.pos() << clk.pos();
		}
	}

  private:
	// simulation of pipelining by adding pipeline registers at output links
	sc_signal <bool> valid_out_pp[N_ROUTER_PORTS][MAX_N_PIPELINE_STAGES];
	sc_signal <Flit> flit_out_pp[N_ROUTER_PORTS][MAX_N_PIPELINE_STAGES];
	sc_signal <bool> in_vc_buffer_rd_pp[N_ROUTER_PORTS][MAX_N_VCs][MAX_N_PIPELINE_STAGES];	// maximum of only one pipeline on the credit back
};

#endif /* WORMHOLE_PIPELINE_H_ */

//================================================
//================ function implementation
