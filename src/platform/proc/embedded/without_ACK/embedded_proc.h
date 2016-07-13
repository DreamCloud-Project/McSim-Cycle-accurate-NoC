/*
 * embedded_proc.h
 *
 *  Created on: Aug 18, 2011
 *      Author: anhttran
 *
 *  Embedded processor without sending ACK when receiving a packet
 */

#ifndef EMBEDDED_PROC_H_
#define EMBEDDED_PROC_H_

#include <queue>
#include <systemc.h>

#include "../../../global_variables.h"
#include "../../../definition.h"
#include "../../virtual_proc.h"
#include "../../../common_functions.h"
#include "../../../router/router_parameters.h"
#include "../../proc_parameters.h"
#include "../../proc_functions.h"

#include "../embedded_parameters.h"
#include "../../proc_evaluation.h"

using namespace std;

class EmbeddedProc: public VirtualProc{
public:
	// location
	int local_x;
	int local_y;

	// initialize
	void initialize(int x, int y);

	// evaluation
	ProcEvaluationFactors *evaluation();

	// keep trace of number of idle entries of each output VC
	sc_signal <int> out_vc_remain[MAX_N_VCS];
	sc_signal <int> out_vc_remain_reg[MAX_N_VCS];

	sc_signal <bool> queue_out_valid;	// queue output before sent to LOCAL port
	sc_signal <Flit> queue_out;

	// processes
	void tx_method();
	void flit_out_method();
	void out_vc_remain_method();	// update out_vc_remain
	void count_plus_method();	// pipelined out_vc_remain
	void out_vc_remain_reg_method();	// pipelined out_vc_remain
	void rx_method();

	// constructor with process enable
	SC_HAS_PROCESS(EmbeddedProc);
	EmbeddedProc (sc_module_name name): VirtualProc(name){

		// send flit
		SC_METHOD (tx_method);
		sensitive << reset.pos() << clk.pos();

		// receive flit
		SC_METHOD (rx_method);
		sensitive << reset.pos() << clk.pos();

		// update out_vc_remain
		SC_METHOD (out_vc_remain_method);
		for (int vo=0; vo<RouterParameter::n_VCs; vo++){
			sensitive << out_vc_remain_reg[vo];
			sensitive << count_plus[vo];
			sensitive << count_minus[vo];
		}

		// pipelined out_vc_remain
		SC_METHOD (out_vc_remain_reg_method);
		sensitive << clk.pos() << reset.pos();

		// count_plus = out_vc_buffer
		SC_METHOD (count_plus_method);
		for (int vo=0; vo<RouterParameter::n_VCs; vo++){
			sensitive << out_vc_buffer_rd[vo];
		}

		// flit_out
		SC_METHOD (flit_out_method);
		sensitive << queue_out_valid << queue_out << out_vc_remain[0];
	}

private:

	sc_signal <bool> count_plus[MAX_N_VCS];
	sc_signal <bool> count_minus[MAX_N_VCS];

	// destination info
	int n_dsts;	// number of destinations <= MAX_N_DESTS
	TileLocation dst_tile_loc[MAX_N_DESTS];

	double flit_inj_rate[MAX_N_DESTS];	// injection rate for each dests

	int injected_packet_count;
	int next_injection_time[MAX_N_DESTS];	// time for injecting next packet

	// an "infinite" queue of injected flits at the source PE
	queue <Flit> source_queue;

	bool was_head[MAX_N_VCS];	// whether a packet head flit was received on a VC
	int received_packet_count;
	double total_latency;
	double max_latency;
	double min_latency;

	// time that the head flit was injected
	int head_injected_time;

};

#endif /* EMBEDDED_PROC_H_ */
