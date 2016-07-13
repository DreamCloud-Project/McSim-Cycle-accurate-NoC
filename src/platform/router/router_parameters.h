/*
 * router_parameter.h
 *
 *  Created on: May 24, 2010
 *      Author: Anh Tran
 *
 *  Define router's parameters
 */

#ifndef ROUTER_PARAMETER_H_
#define ROUTER_PARAMETER_H_

#define ROUTER_WORMHOLE 1	// wormhole with 3 pipelining stages
#define ROUTER_VC 2			// virtual channel
#define ROUTER_EVC 3		// express virtual channel
#define ROUTER_SB_1IB 4		// shared-buffer with 1 buffer per input port
#define ROUTER_SVC 5		// speculative virtual channel
#define ROUTER_LOWCOST_ONE_ENTRY 6	// bufferless router
#define ROUTER_BUFFERLESS_EFC 7	// bufferless router with express flow control
#define ROUTER_WORMHOLE_PIPELINE 8	// wormhole with configurable pipeline stages
#define ROUTER_BUFFERLESS_ADAPTIVE 9 // bufferless router using an adaptive routing policy
#define ROUTER_VC_RT 10			// virtual channel with RT support

#define MAX_N_VCS 16		// maximum number of virtual channel
#define DEFAULT_N_VCS 8			// < MAX_N_VCS

#define NUM_SBs	5		// num of shared buffers

#define DATA_LINK_WIDTH 32
#define FLIT_WIDTH 34		// 2-bit H+T + interconnect bits

//#define COUNTER_SIZE 4		// 4-bit (3+1) counter for 8-entry FIFO
#define DEFAULT_BUFFER_SIZE 16
#define BUFFER_RESERVE 3	// reserved space for avoiding buffer full due to router pipelining
#define VC_BUFFER_RESERVE 3	// reserved space for a virtual-channel buffer with output link pipelining

#define SHARED_BUFFER_SIZE 8

#define VC_BUFFER_TH_OFF 3	// at least 3 if output link is pipelined; at least 1 if not
#define VC_BUFFER_TH_ON 4	// >= TH_OFF + 1

#define ROUTER_PORTS 5		// number of router I/O ports
#define N_ROUTER_PORTS ROUTER_PORTS

#define NOT_REQ -1			// input port does not request any output (empty)
#define NOT_RESERVED -2		// output port is not reserved for any input port yet
#define NOT_ALLOCATED NOT_RESERVED	// input port is not allocated to any output port yet
#define NON_REQ -3			// there is no input requesting an output j
#define NOT_GRANTED -4		// the resource is not granted to any input

#define ROUTING_XY 1
#define ROUTING_SOURCE 2
#define ROUTING_MINIMAL_WEST_FIRST 3
#define ROUTING_MINIMAL_NORTH_LAST 4
#define ROUTING_MINIMAL_NEGATIVE_FIRST 5
#define ROUTING_MINIMAL_ODD_EVEN 6

// which output channel will be chosen if both output channels are okie in minimal routing
#define OUTPUT_CHANNEL_SELECT_NEARER_DIMENSION 1
#define OUTPUT_CHANNEL_SELECT_FARTHER_DIMENSION 2
#define OUTPUT_CHANNEL_SELECT_XY_ORDERED 3	// choose X
#define OUTPUT_CHANNEL_SELECT_ROUNDROBIN 4
#define OUTPUT_CHANNEL_SELECT_HIGHER_CREDIT 5

//#define MAX_ARBITER_INPUTS	5 // the max number of arbiter input port
#define ARBITER_ROUNDROBIN 1

#define VCA_ROUNDROBIN 1
#define VCA_iSLIP 2

#define SA_ROUNDROBIN 1
#define SA_iSLIP 2
#define SA_WINNER_TAKE_ALL 3
#define SA_PRIO 4

#define SBA_ROUNDROBIN 1
#define SBA_iSLIP 2

#define SBA_THRESHOLD 2		// threshold for the shared-buffer allocator
#define SBA_REGULATED 0		// whether allowing input packets form Local port to write to shared buffers

// input buffer states
#define IDLE -3		// having no packet
#define EMPTY IDLE
#define WAIT_FOR_GRANT -2		// packet is waiting for an output VC grant
#define BUSY -1
#define ACTIVE 0	// packet was granted for an out VC
#define ALLOCATED_TO_OUT_PORT 1
#define ALLOCATED_TO_SB 2

// output port and shared-buffer states
#define RESERVED_FOR_IN_BUFFER 1
#define RESERVED_FOR_SB 2

// pipeline stages
#define MIN_N_PIPELINE STAGES 1
#define MAX_N_PIPELINE_STAGES 5

//#define N_PIPELINE_STAGES 3

#define PIPELINE_TYPE_1		1
#define PIPELINE_TYPE_2_1	2
#define PIPELINE_TYPE_2_2	3
#define PIPELINE_TYPE_2_3	4
#define PIPELINE_TYPE_3_1	5
#define PIPELINE_TYPE_3_2	6
#define PIPELINE_TYPE_3_3	7
#define PIPELINE_TYPE_4		8

// router parameters
struct RouterParameter {
	static int router_type;

	static int n_VCs;

	static int n_SBs;

	static int data_link_width;
//	static int flit_size;

	static unsigned int buffer_size;
	static int buffer_reserve;
	static int vc_buffer_th_off;
	static int vc_buffer_th_on;

	static unsigned int shared_buffer_size;

	static int routing_type;
	static int output_channel_select;

	static int packet_length_aware;	// for keep packets in-order in bufferless routers

	static int arbiter_type;
	static int vca_type;
	static int sa_type;

	static int sba_type;
	static int sba_threshold;
	static bool sba_regulated;

	static double link_length; // in um

	static int pipeline_type;

	static int n_pipeline_stages;
};

#endif /* ROUTER_PARAMETER_H_ */
