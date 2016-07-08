/**
 * vc_router.h
 *
 *  Created on: Nov 12, 2010
 *      Author: Anh Tran
 *
 *  Rewrite virtual-channel from scratch with additional features:
 *  	- use template for configuration of N_VCs
 *  	- input VC states: IDLE, WAIT for allocating an out_vc, or ACTIVE with an allocated out_vc
 *  	- output VC states: IDLE, RESERVED for an input VC
 *  	- only consider out_full signals at SA stage
 *
 *  Revised:
 *  	+ 2010.11.18:
 *  		- now, the sw_allocator has two separate grant signals:
 *  			. one port the in_port
 *  			. one port the in_vc
 *  		- credit-based flow control
 *  	+ 2010.12.10:
 *  		. allow to count the number of empty queues
 *  		. count number of cycles a queue is full and empty (after warmup time)
 */

#ifndef VCR_RT_CREDIT_ROUTER_H_
#define VCR_RT_CREDIT_ROUTER_H_

#include <vector>
#include <systemc.h>
#include "../../common_functions.h"
#include "../../MemoryProfiled.h"
#include "../routing_algorithms.h"
#include "../virtual_router.h"

#include "support_modules/in_vc_state_update_rt.h"
#include "support_modules/out_vc_state_update_rt.h"
#include "support_modules/vc_valid_in_demux_rt.h"
#include "vc_rt_buffer.h"
#include "route_compvc_rt.h"
#include "vc_rt_allocator.h"
#include "sw_rt_allocator.h"
#include "vc_rt_crossbar.h"

using namespace std;

#ifdef MEM_PROF
template<int N_VCs>
class VCRTRouter: public VirtualRouter, MemoryProfiled<VCRTRouter<N_VCs> > {
#else
template<int N_VCs>
class VCRTRouter: public VirtualRouter {
#endif

public:
	// local address
	int local_x;
	int local_y;

	// initialize constant values
	void initialize(int x, int y);
	virtual RouterActivities *getRouterActivities();

	//========== internal signals and variables
	sc_signal<int> in_vc_state[N_ROUTER_PORTS][N_VCs];	// input VC state
	sc_signal<int> out_vc_state[N_ROUTER_PORTS][N_VCs];	// output VC state

	sc_signal<bool> vc_valid_in[N_ROUTER_PORTS][N_VCs];
	sc_signal<bool> is_buffer_rd_req[N_ROUTER_PORTS][N_VCs];
	sc_signal<Flit> buffer_out[N_ROUTER_PORTS][N_VCs];
	sc_signal<bool> buffer_empty[N_ROUTER_PORTS][N_VCs];

	sc_signal<bool> buffer_full[N_ROUTER_PORTS][N_VCs];	// used for the paper only

	// out_port_req[pi][vi] = po of NOT_REQ
	sc_signal<int> out_port_req[N_ROUTER_PORTS][N_VCs];
	sc_signal<bool> tail_reg[N_ROUTER_PORTS][N_VCs];

	sc_signal<int> in_vc_state_reg[N_ROUTER_PORTS * N_VCs];
	sc_signal<int> out_vc_state_reg[N_ROUTER_PORTS * N_VCs];
	sc_signal<int> out_port_req_reg[N_ROUTER_PORTS * N_VCs];
	sc_signal<int> vca_priority_stage1[N_ROUTER_PORTS * N_VCs];
	sc_signal<int> vca_priority_stage2[N_ROUTER_PORTS * N_VCs];
	sc_signal<int> vca_priority_stage1_reg[N_ROUTER_PORTS * N_VCs];
	sc_signal<int> vca_priority_stage2_reg[N_ROUTER_PORTS * N_VCs];

	sc_signal<int> vca_grant[N_ROUTER_PORTS * N_VCs];

	// out_vc_allocated[pi][vi] = vo means input vc[pi][vi] is allocated for output vc [vo] of an output port [po]
	// (dont care [po] because this information was given by out_port_req[pi][vi]
	sc_signal<int> out_vc_allocated[N_ROUTER_PORTS][N_VCs];	// which output VC is allocated for an special input VC

	// out_vc_reserved[po][vo] = indexi means the output vc[po][vo] is reserved for an input vc[indexi]=[pi][vi]
//	sc_signal <int> out_vc_reserved[N_ROUTER_PORTS][N_VCs];

	sc_signal<int> sa_priority_stage1[N_ROUTER_PORTS];
	sc_signal<int> sa_priority_stage1_reg[N_ROUTER_PORTS];
	sc_signal<int> sa_priority_stage2[N_ROUTER_PORTS];
	sc_signal<int> sa_priority_stage2_reg[N_ROUTER_PORTS];

	sc_signal<bool> out_full_reg[N_ROUTER_PORTS][N_VCs];
//	sc_signal <int> sa_grant[N_ROUTER_PORTS];	// sa_grant[po] = indexi = [pi][vi]
	sc_signal<int> sa_grant_in_port[N_ROUTER_PORTS];	// = pi
	sc_signal<int> sa_grant_in_vc[N_ROUTER_PORTS];	// = vi

	sc_signal<Flit> crossbar_in_pre[N_ROUTER_PORTS];	// input VC muxed
	sc_signal<bool> crossbar_in_valid_pre[N_ROUTER_PORTS];

	sc_signal<Flit> crossbar_in[N_ROUTER_PORTS];
	sc_signal<bool> crossbar_in_valid[N_ROUTER_PORTS];
	sc_signal<int> crossbar_grant[N_ROUTER_PORTS];

	sc_signal<Flit> crossbar_out[N_ROUTER_PORTS];
	sc_signal<bool> crossbar_out_valid[N_ROUTER_PORTS];

	sc_signal<bool> out_vc_remain_minus[N_ROUTER_PORTS][N_VCs];	// minus if sending a flit
	sc_signal<bool> out_vc_remain_plus[N_ROUTER_PORTS][N_VCs];// pipelined out_vc_buffer_rd[po][vo]

	sc_signal<int> out_vc_remain[N_ROUTER_PORTS][N_VCs]; // number of entries remained in each out VC
	sc_signal<int> out_vc_remain_reg[N_ROUTER_PORTS][N_VCs];

	//========== submodules
	InVCStateUpdateRT *in_vc_state_update[N_ROUTER_PORTS][N_VCs];	// update input VC state
	OutVCStateUpdateRT<N_VCs> *out_vc_state_update[N_ROUTER_PORTS][N_VCs];// update output VC state

	VCValidInDemuxRT *vc_valid_in_demux[N_ROUTER_PORTS][N_VCs];// demuxing valid signals for in VCs
	VCRTBuffer *buffer[N_ROUTER_PORTS][N_VCs];	// each buffer per VC

	RouteCompVcRT *route_comp[N_ROUTER_PORTS][N_VCs];	// output port

	VCRTAllocator<N_VCs> *vc_allocator;	// VCA

	SWRTAllocator<N_VCs> *sw_allocator;	// SA

	VCRTCrossbar<N_VCs> *crossbar;

	//========== processes
	void VCA_input_signals_update_process();// update VCA input signals at each clock edge

	void out_vc_allocated_process();

	void tail_reg_update_process();	// update tail_reg signals at each clock edge

	void SA_input_signals_update_process();	// update SA input signals at each clock edge

	void buffer_rd_req_update_process();

	void crossbar_in_pre_update_process();// input VC muxed at each input port before going to the crossbar

	void crossbar_input_signals_update_process();// update crossbar_in signals at each clock edge

	void lt_process();	// send output to links at each clock edge

	void in_vc_buffer_rd_process();	// pipelined buffer_rd_req before sending back to upstream router

	void out_vc_remain_minus_process();	// decrease counter if sending a flit
	void out_vc_remain_plus_process();	// increase counter if out_vc_buffer_rd

	void out_vc_remain_process();// update number of remained entries in each out VC
	void out_vc_remain_reg_process();	// pipelined out_vc_remain

	// for checking whether a VC is active-> used for count empty queues
	// this information is used for NOCS'11 paper
	void check_in_vc_state_process();

	// count cycles queues are empty and full; used for the paper only
	void count_empty_full_process();

	// constructor with process enable
	SC_HAS_PROCESS(VCRTRouter);
	VCRTRouter(sc_module_name name) :
			VirtualRouter(name) {

		//========== input VC and output VC states update
		string in_vc_state_name = "";
		for (int pi = 0; pi < N_ROUTER_PORTS; pi++) {	// all input ports
			for (int vi = 0; vi < N_VCs; vi++) {	// all VCs of an input port
				int indexi = pi * N_VCs + vi;
				//in_vc_state_name = "in_VC_state[" + int_to_str(pi) + "][" + int_to_str(vi) + "]"; MANUTOREMOVE
				in_vc_state_update[pi][vi] = new InVCStateUpdateRT(
						in_vc_state_name.data());
				in_vc_state_update[pi][vi]->in_port = pi;
				in_vc_state_update[pi][vi]->in_vc = vi;
				in_vc_state_update[pi][vi]->is_empty(buffer_empty[pi][vi]);
				in_vc_state_update[pi][vi]->is_tail_reg(tail_reg[pi][vi]);
				in_vc_state_update[pi][vi]->out_vc_allocated(
						out_vc_allocated[pi][vi]);
				in_vc_state_update[pi][vi]->in_vc_state_reg(
						in_vc_state_reg[indexi]);
				in_vc_state_update[pi][vi]->in_vc_state(in_vc_state[pi][vi]);
			}
		}

		string out_vc_state_name = "";
		for (int po = 0; po < N_ROUTER_PORTS; po++) {	// all output ports
			for (int vo = 0; vo < N_VCs; vo++) {// all VCs of that output port
				int indexo = po * N_VCs + vo;
				//out_vc_state_name = "out_VC_state[" + int_to_str(po) + "]["+ int_to_str(vo) + "]";MANUTOREMOVE
				out_vc_state_update[po][vo] = new OutVCStateUpdateRT<N_VCs>(
						out_vc_state_name.data());
				out_vc_state_update[po][vo]->out_port = po;
				out_vc_state_update[po][vo]->out_vc = vo;
				out_vc_state_update[po][vo]->vca_grant(vca_grant[indexo]);

				for (int pi = 0; pi < N_ROUTER_PORTS; pi++) {// all input ports
					for (int vi = 0; vi < N_VCs; vi++) {// all VCs of that input port
						int indexi = pi * N_VCs + vi;
						out_vc_state_update[po][vo]->tail_reg[indexi](
								tail_reg[pi][vi]);
					}
				}
				out_vc_state_update[po][vo]->out_vc_state_reg(
						out_vc_state_reg[indexo]);
				out_vc_state_update[po][vo]->out_vc_state(out_vc_state[po][vo]);
			}
		}

		//========== stage 1: BW + RC stage
		// input virtual channels DEMUXs
		string vc_valid_name = "";
		for (int pi = 0; pi < N_ROUTER_PORTS; pi++) {	// all input ports
			for (int vi = 0; vi < N_VCs; vi++) {	// all VCs of an input port
				// vc_valid_name = "VC_valid_in[" + int_to_str(pi) + "]["+ int_to_str(vi) + "]";MANUTOREMOVE
				vc_valid_in_demux[pi][vi] = new VCValidInDemuxRT(
						vc_valid_name.data());

				vc_valid_in_demux[pi][vi]->vc_id = vi;
				vc_valid_in_demux[pi][vi]->flit_in(flit_in[pi]);
				vc_valid_in_demux[pi][vi]->valid_in(valid_in[pi]);
				vc_valid_in_demux[pi][vi]->valid_out(vc_valid_in[pi][vi]);
			}
		}

		// N_VCs virtual channel per port; 1 virtual-channel = 1 buffer
		string vc_name = "";
		for (int pi = 0; pi < N_ROUTER_PORTS; pi++) {
			for (int vi = 0; vi < N_VCs; vi++) {	// all virtual channels
				// vc_name = "port=" + int2direction(pi) + ",VC=" + int_to_str(vi); MANUTOREMOVE
				buffer[pi][vi] = new VCRTBuffer(vc_name.data());
				buffer[pi][vi]->initilize(RouterParameter::buffer_size);
				buffer[pi][vi]->port_id = pi;
				buffer[pi][vi]->vc_id = vi;

				// Input signals
				buffer[pi][vi]->clk(clk);
				buffer[pi][vi]->reset(reset);
				buffer[pi][vi]->valid_in(vc_valid_in[pi][vi]);
				buffer[pi][vi]->buffer_in(flit_in[pi]);
				buffer[pi][vi]->rd_req(is_buffer_rd_req[pi][vi]);

				// Output signals
				buffer[pi][vi]->full(buffer_full[pi][vi]);
				buffer[pi][vi]->buffer_out(buffer_out[pi][vi]);
				buffer[pi][vi]->empty(buffer_empty[pi][vi]);
			}
		}

		// Output port computation (RC) - 1 for each input VCs
		string route_comp_name = "";
		for (int pi = 0; pi < N_ROUTER_PORTS; pi++) {
			for (int vi = 0; vi < N_VCs; vi++) {	// all virtual channels
				//route_comp_name = "route_comp[" + int_to_str(pi) + "]["+ int_to_str(vi) + "]"; MANUTOREMOVE
				route_comp[pi][vi] = new RouteCompVcRT(route_comp_name.data());
				route_comp[pi][vi]->in_vc_state(in_vc_state[pi][vi]);
				route_comp[pi][vi]->buffer_empty(buffer_empty[pi][vi]);
				route_comp[pi][vi]->buffer_out(buffer_out[pi][vi]);
				route_comp[pi][vi]->out_port(out_port_req[pi][vi]);
				route_comp[pi][vi]->local_x = local_x;
				route_comp[pi][vi]->local_y = local_y;
				route_comp[pi][vi]->portId = pi;
				route_comp[pi][vi]->vcId = vi;
			}
		}

		//============= stage2: VCA
		// register signals from stage1 to stage2
		SC_METHOD(VCA_input_signals_update_process);
		sensitive << clk.pos() << reset.pos();

		SC_METHOD(tail_reg_update_process);
		sensitive << clk.pos() << reset.pos();

		// VCA - 1 single VCA for all the ports
		vc_allocator = new VCRTAllocator<N_VCs>("VC_Allocator");
		for (int pi = 0; pi < N_ROUTER_PORTS; pi++) {
			for (int vi = 0; vi < N_VCs; vi++) { // all input VCs
				int indexi = pi * N_VCs + vi;
				vc_allocator->out_port_req_reg[indexi](
						out_port_req_reg[indexi]);
				vc_allocator->tail_reg[indexi](tail_reg[pi][vi]);
				vc_allocator->in_vc_state_reg[indexi](in_vc_state_reg[indexi]);
				vc_allocator->vca_priority_stage1_reg[indexi](
						vca_priority_stage1_reg[indexi]);
				vc_allocator->vca_priority_stage1_new[indexi](
						vca_priority_stage1[indexi]);
			}
		}
		for (int po = 0; po < N_ROUTER_PORTS; po++) { // all output port
			for (int vo = 0; vo < N_VCs; vo++) { // all output VCs of that port
				int indexo = po * N_VCs + vo;
				vc_allocator->out_vc_state_reg[indexo](
						out_vc_state_reg[indexo]);
				vc_allocator->vca_priority_stage2_reg[indexo](
						vca_priority_stage2_reg[indexo]);
				vc_allocator->vca_priority_stage2_new[indexo](
						vca_priority_stage2[indexo]);
				vc_allocator->vca_grant[indexo](vca_grant[indexo]);
			}
		}

		// update out_vc_allocated for each input VC
		SC_METHOD(out_vc_allocated_process);
		for (int indexo = 0; indexo < N_ROUTER_PORTS * N_VCs; indexo++) {
			sensitive << vca_grant[indexo];
		}

		//============== stage3: SA
		// update input signals at each clock edge
		SC_METHOD(SA_input_signals_update_process);
		sensitive << clk.pos() << reset.pos();

		// SA  - 1 single SA for all the ports
		sw_allocator = new SWRTAllocator<N_VCs>("SW_Allocator");
		for (int pi = 0; pi < N_ROUTER_PORTS; pi++) {
			for (int vi = 0; vi < N_VCs; vi++) {
				int indexi = pi * N_VCs + vi;
				sw_allocator->in_vc_state_reg[pi][vi](in_vc_state_reg[indexi]);
				sw_allocator->out_port_req_reg[pi][vi](
						out_port_req_reg[indexi]);
				sw_allocator->tail_reg[pi][vi](tail_reg[pi][vi]);
			}
			sw_allocator->sa_priority_stage1_reg[pi](
					sa_priority_stage1_reg[pi]);
			sw_allocator->sa_priority_stage1_new[pi](sa_priority_stage1[pi]);
		}
		for (int po = 0; po < N_ROUTER_PORTS; po++) {
			for (int vo = 0; vo < N_VCs; vo++) {
				sw_allocator->out_vc_remain_reg[po][vo](
						out_vc_remain_reg[po][vo]);
				sw_allocator->out_vc_remain_minus[po][vo](
						out_vc_remain_minus[po][vo]);
				sw_allocator->out_vc_remain_plus[po][vo](
						out_vc_remain_plus[po][vo]);
			}
			sw_allocator->sa_grant_in_port[po](sa_grant_in_port[po]);
			sw_allocator->sa_grant_in_vc[po](sa_grant_in_vc[po]);
			sw_allocator->sa_priority_stage2_reg[po](
					sa_priority_stage2_reg[po]);
			sw_allocator->sa_priority_stage2_new[po](sa_priority_stage2[po]);
		}

		// update buffer_rd_req signals
		SC_METHOD(buffer_rd_req_update_process);
		for (int pi = 0; pi < N_ROUTER_PORTS; pi++) {
			for (int vi = 0; vi < N_VCs; vi++) {
				sensitive << buffer_empty[pi][vi];
			}
		}
		for (int po = 0; po < N_ROUTER_PORTS; po++) {
//			sensitive << sa_grant[po];
			sensitive << sa_grant_in_port[po];
			sensitive << sa_grant_in_vc[po];
		}

		// update crossbar_in_pre signals based on buffer_rd_req signals
		SC_METHOD(crossbar_in_pre_update_process);
		for (int pi = 0; pi < N_ROUTER_PORTS; pi++) {
			for (int vi = 0; vi < N_VCs; vi++) {
				sensitive << is_buffer_rd_req[pi][vi];
				sensitive << buffer_out[pi][vi];
				sensitive << out_vc_allocated[pi][vi]; // for update vc_id
			}
		}

		//=========== stage4: ST
		SC_METHOD(crossbar_input_signals_update_process);
		sensitive << clk.pos() << reset.pos();

		// crossbar travel
		crossbar = new VCRTCrossbar<N_VCs>("Crossbar");
		for (int pi = 0; pi < N_ROUTER_PORTS; pi++) {
			crossbar->crossbar_in[pi](crossbar_in[pi]);
			crossbar->crossbar_in_valid[pi](crossbar_in_valid[pi]);
		}
		for (int po = 0; po < N_ROUTER_PORTS; po++) {
			crossbar->crossbar_grant[po](crossbar_grant[po]);
			crossbar->crossbar_out[po](crossbar_out[po]);
			crossbar->crossbar_out_valid[po](crossbar_out_valid[po]);
		}

		//============ stage5: LT
		SC_METHOD(lt_process);
		sensitive << reset.pos() << clk.pos();

		//=========== out_vc_cout update
		// update counter_minus[po][vo] signal
		SC_METHOD(out_vc_remain_minus_process);
		for (int pi = 0; pi < N_ROUTER_PORTS; pi++) {
			sensitive << crossbar_in_valid[pi];
			sensitive << crossbar_in[pi];
		}
		for (int po = 0; po < N_ROUTER_PORTS; po++) {
			sensitive << crossbar_grant[po];
		}

		// counter_plus is pipelined out_vc_buffer_rd
		SC_METHOD(out_vc_remain_plus_process);
		sensitive << clk.pos() << reset.pos();

		// update out_vc_remain count
		SC_METHOD(out_vc_remain_process);
		for (int po = 0; po < N_ROUTER_PORTS; po++) {
			for (int vo = 0; vo < N_VCs; vo++) {
				sensitive << out_vc_remain_minus[po][vo];
				sensitive << out_vc_remain_plus[po][vo];
				sensitive << out_vc_remain_reg[po][vo];
			}
		}

		// pipelined out_vc_remain
		SC_METHOD(out_vc_remain_reg_process);
		sensitive << clk.pos() << reset.pos();

		// sending credit back to upstream router
		SC_METHOD(in_vc_buffer_rd_process);
		sensitive << clk.pos() << reset.pos();

		// checking in VCs to count empty_queues
		SC_METHOD(check_in_vc_state_process);
		for (int pi = 0; pi < N_ROUTER_PORTS; pi++) {
			for (int vi = 0; vi < N_VCs; vi++) {
				sensitive << in_vc_state[pi][vi];
			}
		}
		sensitive << reset;

		// count number of cycles queues are full or empty
		SC_METHOD(count_empty_full_process);
		sensitive << reset.pos() << clk.pos();
	}

private:
	bool in_vc_checked[N_ROUTER_PORTS][N_VCs];// whether this VC is checked for empty or not
	int n_empty_queues = 0;
	int n_empty_cycles[N_ROUTER_PORTS][N_VCs];
	int n_full_cycles[N_ROUTER_PORTS][N_VCs];
};

#endif /* VCR_CREDIT_ROUTER_H_ */

//===================================================================
//======================= function Implementation====================
/* initialize
 *
 */
template<int N_VCs>
void VCRTRouter<N_VCs>::initialize(int x, int y) {
	local_x = x;
	local_y = y;

	// setup local location for route_comp modules
	for (int pi = 0; pi < N_ROUTER_PORTS; pi++) {
		for (int vi = 0; vi < N_VCs; vi++) {
			route_comp[pi][vi]->local_x = x;
			route_comp[pi][vi]->local_y = y;
		}
	}

//	n_empty_queues = N_ROUTER_PORTS*N_VCs;
	for (int pi = 0; pi < N_ROUTER_PORTS; pi++) {
		for (int vi = 0; vi < N_VCs; vi++) {
			in_vc_checked[pi][vi] = 0;
		}
	}
}

/*
 * data evaluation, used for writing papers
 */
/*
 template <int N_VCs>
 RouterEvaluation *VCRouter<N_VCs>::evaluation(){
 RouterEvaluation *eval = new RouterEvaluation();
 eval->n_empty_queues = n_empty_queues;

 return eval;
 }
 */

/*
 * update input signals for VCA at each clock edge
 */
template<int N_VCs>
void VCRTRouter<N_VCs>::VCA_input_signals_update_process() {

	if (reset.read()) {	// reset
		for (int pi = 0; pi < N_ROUTER_PORTS; pi++) {
			for (int vi = 0; vi < N_VCs; vi++) {
				int indexi = pi * N_VCs + vi;
				in_vc_state_reg[indexi].write(IDLE);
				out_port_req_reg[indexi].write(NOT_REQ);
				vca_priority_stage1_reg[indexi].write(0);// initialize different priority for different arbiter
			}
		}

		for (int po = 0; po < N_ROUTER_PORTS; po++) {
			for (int vo = 0; vo < N_VCs; vo++) {
				int indexo = po * N_VCs + vo;
				vca_priority_stage2_reg[indexo].write(0);
				out_vc_state_reg[indexo].write(IDLE);
			}
		}
	}

	else {	// clk edge
		for (int pi = 0; pi < N_ROUTER_PORTS; pi++) {
			for (int vi = 0; vi < N_VCs; vi++) {
				int indexi = pi * N_VCs + vi;
				in_vc_state_reg[indexi].write(in_vc_state[pi][vi].read());
				out_port_req_reg[indexi].write(out_port_req[pi][vi].read());
				vca_priority_stage1_reg[indexi].write(
						vca_priority_stage1[indexi].read());
			}
		}

		for (int po = 0; po < N_ROUTER_PORTS; po++) {
			for (int vo = 0; vo < N_VCs; vo++) {
				int indexo = po * N_VCs + vo;
				vca_priority_stage2_reg[indexo].write(
						vca_priority_stage2[indexo].read());
				out_vc_state_reg[indexo].write(out_vc_state[po][vo].read());
			}
		}
	}
}

/*
 * update tail_reg signals
 */
template<int N_VCs>
void VCRTRouter<N_VCs>::tail_reg_update_process() {
	if (reset.read()) {	// if reset
		for (int pi = 0; pi < N_ROUTER_PORTS; pi++) {
			for (int vi = 0; vi < N_VCs; vi++) {
				tail_reg[pi][vi].write(0);
			}
		}
	} else {	// if clock edge
		for (int pi = 0; pi < N_ROUTER_PORTS; pi++) {
			for (int vi = 0; vi < N_VCs; vi++) {
				if (is_buffer_rd_req[pi][vi].read())
					tail_reg[pi][vi].write(buffer_out[pi][vi].read().tail);
				else
					tail_reg[pi][vi].write(0);
			}
		}
	}
}

/*
 * update out_vc_allocated for each input Vc after VC allocation
 */
template<int N_VCs>
void VCRTRouter<N_VCs>::out_vc_allocated_process() {
	for (int pi = 0; pi < N_ROUTER_PORTS; pi++) {
		for (int vi = 0; vi < N_VCs; vi++) {		// check all input VCs
			int indexi = pi * N_VCs + vi;
			bool check = 0;
			for (int po = 0; po < N_ROUTER_PORTS; po++) {
				for (int vo = 0; vo < N_VCs; vo++) {	// check all output VCs
					int indexo = po * N_VCs + vo;
					if (vca_grant[indexo].read() == indexi) {
						out_vc_allocated[pi][vi].write(vo);
						check = 1;
						break;
					}
				}

				if (check == 1)
					break;
			}

			if (check == 0) {
				out_vc_allocated[pi][vi].write(NOT_ALLOCATED);
			}
		}
	}

}

/*
 * update input signals for SA stage at each clock edge
 */
template<int N_VCs>
void VCRTRouter<N_VCs>::SA_input_signals_update_process() {
	if (reset.read()) {	// reset
		for (int pi = 0; pi < N_ROUTER_PORTS; pi++) {
			sa_priority_stage1_reg[pi].write(0);	// initialized priority
		}
		for (int po = 0; po < N_ROUTER_PORTS; po++) {
//			sa_priority_stage2_reg[po].write(po);	// different priority
			sa_priority_stage2_reg[po].write(0);	// different priority
//			for (int vo=0; vo<N_VCs; vo++){
//				out_full_reg[po][vo].write(0);
//			}
		}
	} else {	// clock edge
		for (int pi = 0; pi < N_ROUTER_PORTS; pi++) {
			sa_priority_stage1_reg[pi].write(sa_priority_stage1[pi].read());
		}
		for (int po = 0; po < N_ROUTER_PORTS; po++) {
			sa_priority_stage2_reg[po].write(sa_priority_stage2[po].read());
//			for (int vo=0; vo<N_VCs; vo++){
//				out_full_reg[po][vo].write(out_full[po][vo].read());
//			}
		}

	}
}

/*
 * update buffer_rd_req signals according to sa_grant and empty signals
 */
template<int N_VCs>
void VCRTRouter<N_VCs>::buffer_rd_req_update_process() {
	for (int pi = 0; pi < N_ROUTER_PORTS; pi++) {
		for (int vi = 0; vi < N_VCs; vi++) {
//			int indexi = pi*N_VCs + vi;
			bool check = 0;
			for (int po = 0; po < N_ROUTER_PORTS; po++) {
				if ((sa_grant_in_port[po].read() == pi)
						&& (sa_grant_in_vc[po].read() == vi)
						&& (!buffer_empty[pi][vi].read())) {
					is_buffer_rd_req[pi][vi].write(1);
					check = 1;
					break;
				}
			}

			if (check == 0) {
				is_buffer_rd_req[pi][vi].write(0);
			}
		}
	}
}

/*
 * update crossbar_in_req signals based on buffer_rd_req signals
 */
template<int N_VCs>
void VCRTRouter<N_VCs>::crossbar_in_pre_update_process() {
	for (int pi = 0; pi < N_ROUTER_PORTS; pi++) {
		bool check = 0;
		for (int vi = 0; vi < N_VCs; vi++) {
			if (is_buffer_rd_req[pi][vi].read()) {
				Flit flit_tmp = buffer_out[pi][vi].read();
				// TODO Keep same VC
				flit_tmp.vc_id = out_vc_allocated[pi][vi].read();// update new vc_id
				crossbar_in_pre[pi].write(flit_tmp);
				crossbar_in_valid_pre[pi].write(1);
				check = 1;
				break;
			}
		}

		if (check == 0) {
			crossbar_in_pre[pi].write(Flit());
			crossbar_in_valid_pre[pi].write(0);
		}
	}
}

/*
 * update crossbar_in signals at each clock edge
 */
template<int N_VCs>
void VCRTRouter<N_VCs>::crossbar_input_signals_update_process() {
	if (reset.read()) {	// reset
		for (int pi = 0; pi < N_ROUTER_PORTS; pi++) {
			crossbar_in[pi].write(Flit());
			crossbar_in_valid[pi].write(0);
		}
		for (int po = 0; po < N_ROUTER_PORTS; po++) {
			crossbar_grant[po].write(NOT_GRANTED);
		}
	} else {	// clock edge
		for (int pi = 0; pi < N_ROUTER_PORTS; pi++) {
			crossbar_in[pi].write(crossbar_in_pre[pi].read());
			crossbar_in_valid[pi].write(crossbar_in_valid_pre[pi].read());
		}
		for (int po = 0; po < N_ROUTER_PORTS; po++) {
//			crossbar_grant[po].write(sa_grant[po].read());	// = indexi
			crossbar_grant[po].write(sa_grant_in_port[po].read());	// = pi
		}
	}
}

/*
 * send output to links at each clock edge
 */
template<int N_VCs>
void VCRTRouter<N_VCs>::lt_process() {
	if (reset.read()) {	// reset
		for (int po = 0; po < N_ROUTER_PORTS; po++) {
			flit_out[po].write(Flit());
			valid_out[po].write(0);
		}
	} else {	// clock edge
		for (int po = 0; po < N_ROUTER_PORTS; po++) {
			flit_out[po].write(crossbar_out[po].read());
			valid_out[po].write(crossbar_out_valid[po].read());
		}
	}
}

/*
 * update counter_minus signals after sending a flit
 */
template<int N_VCs>
void VCRTRouter<N_VCs>::out_vc_remain_minus_process() {
	for (int po = 0; po < N_ROUTER_PORTS; po++) {
		for (int vo = 0; vo < N_VCs; vo++) {
			int pi = crossbar_grant[po].read();
			if ((pi >= 0) && crossbar_in_valid[pi].read()
					&& (crossbar_in[pi].read().vc_id == vo)) {
				out_vc_remain_minus[po][vo].write(1);
			} else
				out_vc_remain_minus[po][vo].write(0);
		}
	}
}

/*
 * update counter_minus each clock edge
 */
template<int N_VCs>
void VCRTRouter<N_VCs>::out_vc_remain_plus_process() {
	if (reset.read()) {	// reset
		for (int po = 0; po < N_ROUTER_PORTS; po++) {
			for (int vo = 0; vo < N_VCs; vo++) {
				out_vc_remain_plus[po][vo].write(0);
			}
		}
	} else {	// clk edge
		for (int po = 0; po < N_ROUTER_PORTS; po++) {
			for (int vo = 0; vo < N_VCs; vo++) {
				out_vc_remain_plus[po][vo].write(
						out_vc_buffer_rd[po][vo].read());
			}
		}
	}
}

/*
 * update out_vc_remain
 */
template<int N_VCs>
void VCRTRouter<N_VCs>::out_vc_remain_process() {
	for (int po = 0; po < N_ROUTER_PORTS; po++) {
		for (int vo = 0; vo < N_VCs; vo++) {
			int tmp = out_vc_remain_reg[po][vo].read();
			if (out_vc_remain_minus[po][vo].read())
				tmp -= 1;
			if (out_vc_remain_plus[po][vo].read())
				tmp += 1;

			out_vc_remain[po][vo].write(tmp);
		}
	}
}

/*
 * pipelined out_vc_remain_reg
 */
template<int N_VCs>
void VCRTRouter<N_VCs>::out_vc_remain_reg_process() {
	if (reset.read()) {	// reset
		for (int po = 0; po < N_ROUTER_PORTS; po++) {
			for (int vo = 0; vo < N_VCs; vo++) {
				out_vc_remain_reg[po][vo].write(
						(int) RouterParameter::buffer_size);
			}
		}
	} else {	// clk edge
		for (int po = 0; po < N_ROUTER_PORTS; po++) {
			for (int vo = 0; vo < N_VCs; vo++) {
				out_vc_remain_reg[po][vo].write(out_vc_remain[po][vo].read());
			}
		}
	}
}

/*
 * sending credit = in_vc_buffer_rd
 */
template<int N_VCs>
void VCRTRouter<N_VCs>::in_vc_buffer_rd_process() {
	if (reset.read()) {	// reset
		for (int pi = 0; pi < N_ROUTER_PORTS; pi++) {
			for (int vi = 0; vi < N_VCs; vi++) {
				in_vc_buffer_rd[pi][vi].write(0);
			}
		}
	} else {	// clk edge
		for (int pi = 0; pi < N_ROUTER_PORTS; pi++) {
			for (int vi = 0; vi < N_VCs; vi++) {
				in_vc_buffer_rd[pi][vi].write(is_buffer_rd_req[pi][vi].read());
			}
		}
	}
}

/*
 * check in_vc_state to count empty queues
 */
template<int N_VCs>
void VCRTRouter<N_VCs>::check_in_vc_state_process() {
	if (reset.read()) {
		n_empty_queues = N_ROUTER_PORTS * N_VCs;
	} else {
		for (int pi = 0; pi < N_ROUTER_PORTS; pi++) {
			for (int vi = 0; vi < N_VCs; vi++) {
				if (!in_vc_checked[pi][vi]
						&& (in_vc_state[pi][vi].read() != EMPTY)) {
					in_vc_checked[pi][vi] = 1;
					n_empty_queues -= 1;
//					cout << "n_empty_queues[" << local_x << "][" << local_y << "] now = " << n_empty_queues << endl;
				}
			}
		}
	}
}

/*
 * check in_vc_state to count empty queues
 */
template<int N_VCs>
void VCRTRouter<N_VCs>::count_empty_full_process() {
	if (reset.read()) {
		for (int pi = 0; pi < N_ROUTER_PORTS; pi++) {
			for (int vi = 0; vi < N_VCs; vi++) {
				n_empty_cycles[pi][vi] = 0;
				n_full_cycles[pi][vi] = 0;
			}
		}
	} else {
		int current_time = (int) (sc_time_stamp().to_double() / 1000);
		if (current_time >= CommonParameter::warmup_time) {	// consider after warmup time
			for (int pi = 0; pi < N_ROUTER_PORTS; pi++) {
				for (int vi = 0; vi < N_VCs; vi++) {
					if (buffer_empty[pi][vi].read())
						n_empty_cycles[pi][vi] += 1;
					if (buffer_full[pi][vi].read())
						n_full_cycles[pi][vi] += 1;
				}
			}
		}
	}
}

template<int N_VCs>
RouterActivities * VCRTRouter<N_VCs>::getRouterActivities() {
	return NULL;
}

