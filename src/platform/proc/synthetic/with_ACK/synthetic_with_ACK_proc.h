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
 *  	+ 2011.07.27: add ACK support for in-order packet delivery with adaptive routing
 *  	+ 2011.08.08:
 *  		- if SENDING_ACK_ALWAYS --> the source requires its destination to send ACK each time it receives a packet
 *  		- if SENDING_ACK_OPTIMAL --> only require the destination to send ACK if the packet length >= ceil(m/2)+1 flits
 *  			where n is the distance between source and destination
 */

#ifndef SYNTHETIC_WITH_ACK_PROC_H_
#define SYNTHETIC_WITH_ACK_PROC_H_

//#include <math.h>
#include <queue>
#include <systemc.h>
#include "../../../definition.h"
#include "../../virtual_proc.h"
#include "../../../common_functions.h"
#include "../../../router/router_parameters.h"
#include "../synthetic_parameters.h"
//#include "../../out_vc_arbiter.h"

#include "../../../global_variables.h"

#include "../../proc_parameters.h"
#include "../../proc_functions.h"
#include "../../proc_evaluation.h"

#define TX_OKIE_TO_SEND_EITHER_DATA_OR_ACK 0
#define TX_OKIE_TO_SEND_DATA 1
#define TX_OKIE_TO_SEND_ACK 2
#define TX_WAIT_FOR_ACK TX_OKIE_TO_SEND_ACK

using namespace std;

class SyntheticWithACKProc: public VirtualProc{
  public:

	// local address
	int local_x;
	int local_y;
/*
	// clk and reset
	sc_in <bool> clk;
	sc_in <bool> reset;

	// Input interface
	sc_in <bool> valid_in;
	sc_in <Flit> flit_in;
	sc_out <bool> in_full;

	// output interface
	sc_out <bool> valid_out;
	sc_out <Flit> flit_out;
	sc_in <bool> out_full;
*/

	// functions
	void initialize(int x, int y);

	// for evaluation
//	SyntheticFactors *synthetic_evaluation();
//	AsAPFactors *asap_evaluation(){return NULL;};
	ProcEvaluationFactors *evaluation();

//	OutVCArbiter *vc_arbiter;	// choose VC of the local router port

	// signals
//	sc_signal <bool> tail_reg;	// for supporting vc_arbiter
//	sc_signal <int> out_vc_grant;

	sc_signal <int> out_vc_remain[MAX_N_VCS];		// keep trace of number of idle entries of each output VC
	sc_signal <int> out_vc_remain_reg[MAX_N_VCS];

	sc_signal <bool> count_plus[MAX_N_VCS];	// = out_vc_buffer_rd
	sc_signal <bool> count_minus[MAX_N_VCS];

	sc_signal <bool> source_queue_out_valid;	// queue output before sent to LOCAL port
	sc_signal <Flit> source_queue_out;

	sc_signal <bool> ack_queue_out_valid;
	sc_signal <Flit> ack_queue_out;

	sc_signal <int> tx_state;	// state of the tx: whether it is okie to send a new packet or wait for an ACK
	sc_signal <int> tx_state_reg;

	sc_signal <bool> has_sent_tail_flit;	// whether the tx has sent a packet
	sc_signal <bool> has_sent_head_flit;
	sc_signal <bool> has_received_ACK;	// whether the proc has received a ACK at the last cycle

	sc_signal <bool> received_ACK;	// whether the proc received a ACK before
	sc_signal <bool> received_ACK_reg;

	sc_signal <int> tx_mux_sel; // choose to send data flit or ACK flit

//	sc_signal <int> sent_packet_length;	// used for optimal ACK control
//	sc_signal <int> sent_packet_length_reg;

	sc_signal <bool> is_ACK_required; // used for optimal ACK control

	// processes
	void tx_state_process();	// update tx_state
	void tx_state_reg_process();

	void tx_mux_sel_process();	// choose to send data flit or ACK flit

	void has_sent_tail_flit_process();
	void has_sent_head_flit_process();

	void tx_process();

	void flit_out_process();

	void rx_process();

	void received_ACK_process();
	void received_ACK_reg_process();

	void count_minus_process();	// update count_minus
	void count_plus_process();	// pipelined out_vc_remain

	void out_vc_remain_process();	// update out_vc_remain
	void out_vc_remain_reg_process();	// pipelined out_vc_remain

//	void sent_packet_length_process();
//	void sent_packet_length_reg_process();

	// constructor with process enable
	SC_HAS_PROCESS(SyntheticWithACKProc);
	SyntheticWithACKProc (sc_module_name name): VirtualProc(name){
		// update tx_state
		SC_METHOD (tx_state_process);
		sensitive << tx_state_reg;
//		sensitive << source_queue_out_valid << ack_queue_out_valid;
		sensitive << has_sent_tail_flit;
		sensitive << has_sent_head_flit;
		sensitive << has_received_ACK;
		sensitive << received_ACK;
		sensitive << is_ACK_required;

		// update tx_state_reg
		SC_METHOD (tx_state_reg_process);
		sensitive << reset.pos() << clk.pos();

		// update tx_mux_sel
		SC_METHOD (tx_mux_sel_process);
		sensitive << out_vc_remain[0];
		sensitive << source_queue_out_valid;
		sensitive << ack_queue_out_valid;
		sensitive << tx_state;

		// has_sent_tail_flit update
		SC_METHOD (has_sent_tail_flit_process);
		sensitive << reset.pos() << clk.pos();

		// has_sent_head_flit update
		SC_METHOD (has_sent_head_flit_process);
		sensitive << reset.pos() << clk.pos();

		// send flit
		SC_METHOD (tx_process);
		sensitive << reset.pos() << clk.pos();

		// flit_out
		SC_METHOD (flit_out_process);
//		sensitive << source_queue_out_valid << source_queue_out << out_vc_remain[0];
		sensitive << tx_mux_sel;
		sensitive << source_queue_out;
		sensitive << ack_queue_out;

		// receive flit
		SC_METHOD (rx_process);
		sensitive << reset.pos() << clk.pos();

		// received_ACK
		SC_METHOD (received_ACK_process);
		sensitive << received_ACK_reg;
		sensitive << has_sent_head_flit;
		sensitive << has_received_ACK;

		// received_ACK_reg
		SC_METHOD (received_ACK_reg_process);
		sensitive << clk.pos() << reset.pos();


/*
		// vc_arbiter
		vc_arbiter = new OutVCArbiter("Out_Local_VC_Arbiter");
		vc_arbiter->clk(clk);
		vc_arbiter->reset(reset);
		vc_arbiter->tail_reg(tail_reg);
		for (int k=0; k<MAX_N_VCS; k++){
			vc_arbiter->out_vc_remain[k](out_vc_remain[k]);
//			vc_arbiter->out_vc_remain_reg[k](out_vc_remain_reg[k]);
//			vc_arbiter->count_minus[k](count_minus[k]);
//			vc_arbiter->count_plus[k](count_plus[k]);
		}
		vc_arbiter->grant(out_vc_grant);
*/

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

		// count_minus update (only when sending a flit)
		SC_METHOD (count_minus_process);
		sensitive << clk.pos() << reset.pos();

/*
		// sent_packet_length
		SC_METHOD (sent_packet_length_process);
		sensitive << has_sent_head_flit;
		sensitive << sent_packet_length_reg;
*/
	}

	// functions
  private:
	bool was_head[MAX_N_VCS];	// whether a packet head flit was received on a VC
	int injected_packet_count;
	int received_packet_count;
	double total_latency;
	double max_latency;
	double min_latency;

	// packet inter-injection time (cycles) with exponential distribution
//	int inter_injection_time(double flit_rate, int type);

	int next_injection_time;	// time for injecting next packet

	// a "infinite" queue of injected flits at the source PE
	queue <Flit> source_queue;

	queue <Flit> ack_queue;	// queue for ACK flits in case the output is busy

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

//	Flit *create_head_flit_bit_reversal(int _src_x, int _src_y, int _time);

//	Flit *create_head_flit_tornado(int _src_x, int _src_y, int _time);

//	Flit *create_head_flit_ACK(int _src_x, int _src_y, int _dst_x, int _dst_y, int _time);
};

#endif /* SYNTHETIC_WITH_ACK_PROC_H_ */
