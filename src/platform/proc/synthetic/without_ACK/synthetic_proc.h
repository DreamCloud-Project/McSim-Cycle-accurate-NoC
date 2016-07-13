/*
 * pe.h
 *
 *  Created on: Apr 16, 2010
 *      Author: Anh Tran, VCL, UC Davis
 *
 *  Model of a processing element (PE)
 *  for injecting and consuming packet into/from the network
 *
 *  Revised:
 *  	+ Oct 25, 2010: now supports virtual-channel router
 *  	+ 2010.11.18: support credit-based flow control
 *  	+ 2010.12.06: add tornado traffic
 */

#ifndef SYNTHETIC_PROC_H_
#define SYNTHETIC_PROC_H_

//#include <math.h>
#include <queue>
#include <systemc.h>
#include "../../../definition.h"
#include "../../virtual_proc.h"
#include "../../../common_functions.h"
#include "../../../router/router_parameters.h"
#include "../../proc_parameters.h"
#include "../synthetic_parameters.h"
//#include "../../out_vc_arbiter.h"

#include "../../../global_variables.h"

#include "../../proc_functions.h"
#include "../../proc_evaluation.h"

using namespace std;

class SyntheticProc: public VirtualProc{
  public:

	// local address
	int local_x;
	int local_y;

	// functions
	void initialize(int x, int y);

	// for evaluation
	ProcEvaluationFactors *evaluation();

	// processes
	void tx_process();

	void flit_out_process();

	void out_vc_remain_process();	// update out_vc_remain

	void count_plus_process();	// pipelined out_vc_remain
	void out_vc_remain_reg_process();	// pipelined out_vc_remain

	void rx_process();

	// constructor with process enable
	SC_HAS_PROCESS(SyntheticProc);
	SyntheticProc (sc_module_name name): VirtualProc(name){
		// send flit
		SC_METHOD (tx_process);
		sensitive << reset.pos() << clk.pos();

		// receive flit
		SC_METHOD (rx_process);
		sensitive << reset.pos() << clk.pos();

		// update out_vc_remain
		SC_METHOD (out_vc_remain_process);
		for (int vo=0; vo<RouterParameter::n_VCs; vo++){
			sensitive << out_vc_remain_reg[vo];
			sensitive << count_plus[vo];
//			sensitive << out_vc_buffer_rd[vo];
			sensitive << count_minus[vo];
		}

		// pipelined out_vc_remain
		SC_METHOD (out_vc_remain_reg_process);
		sensitive << clk.pos() << reset.pos();

		// count_plus = out_vc_buffer
		SC_METHOD (count_plus_process);
		for (int vo=0; vo<RouterParameter::n_VCs; vo++){
			sensitive << out_vc_buffer_rd[vo];
//			sensitive << out_port_EFC_en;
		}

		// flit_out
		SC_METHOD (flit_out_process);
		sensitive << queue_out_valid << queue_out << out_vc_remain[0];
	}

	// functions
  private:
	bool was_head[MAX_N_VCS];	// whether a packet head flit was received on a VC
	int injected_packet_count;
	int received_packet_count;
	double total_latency;
	double max_latency;
	double min_latency;

	sc_signal <bool> queue_out_valid;	// queue output before sent to LOCAL port
	sc_signal <Flit> queue_out;

	sc_signal <int> out_vc_remain[MAX_N_VCS];		// keep trace of number of idle entries of each output VC
	sc_signal <int> out_vc_remain_reg[MAX_N_VCS];

	sc_signal <bool> count_plus[MAX_N_VCS];	// = out_vc_buffer_rd
	sc_signal <bool> count_minus[MAX_N_VCS];

	// packet inter-injection time (cycles) with exponential distribution
//	int inter_injection_time(double flit_rate, int type);

	int next_injection_time;	// time for injecting next packet

	// An "infinite" queue of flits to be injected into the NoC
	queue <Flit> source_queue;

//	// where the source queue is full;
//	bool is_source_queue_full();

	// time that the head flit was injected
	int head_injected_time;

	// time that the tail flit of the same packet is receiving
	int tail_receiving_time;

	// generate a head flit with random destination
//	Flit *create_head_flit_random(int _src_x, int _src_y, int _time);
	// generate a head flit with transpose destination
//	Flit *create_head_flit_transpose(int _src_x, int _src_y, int _time);
	// generate a head flit with bit-reversal destination
//	Flit *create_head_flit_bit_complement(int _src_x, int _src_y, int _time);

//	Flit *create_head_flit_tornado(int _src_x, int _src_y, int _time);

//	Flit *create_head_flit_ACK(int _src_x, int _src_y, int _dst_x, int _dst_y, int _time);
};

#endif /* SYNTHETIC_PROC_H_ */
