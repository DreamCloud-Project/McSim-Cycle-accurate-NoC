/*
 * parameters.h
 *
 *  Created on: Apr 12, 2010
 *      Author: Anh Tran, VCL, UC Davis
 *
 *  Defines parameters (reconfigurable) for routers
 */

#ifndef COMMON_PARAMETER_H_
#define COMMON_PARAMETER_H_

#include <time.h>
#include <string>
#include <iostream>

using namespace std;

//======== default parameters
#define SIM_MODE_CYCLE 0
#define SIM_MODE_PACKET 1

#define SIMULATION_TIME 100000
#define WARMUP_TIME 20000

#define MAX_N_RX_PACKETS 100000

// default of x and y dimension sizes
#define DIM_X 8
#define DIM_Y 8
#define MAX_DIM 21		// maximum of 20x20 = 400 tiles, used for initializing the NoC

// directions
#define WEST 0
#define EAST 1
#define NORTH 2
#define SOUTH 3
#define LOCAL 4
#define WEST_NORTH 5	// for profitable adaptive routing
#define WEST_SOUTH 6
#define EAST_NORTH 7
#define EAST_SOUTH 8

#define PLATFORM_SYNTHETIC 1
#define PLATFORM_EMBEDDED 2
#define PLATFORM_DC 3


#define MAX_PACKET_LENGTH 50 // used for initializing a packet

#define MAX_N_TASKS 100	// number of tasks in an embedded or asap applications


#define CLK_FREQ_FIXED 1
#define CLK_FREQ_MAX 2

// global common parameters
struct CommonParameter{
	static int sim_mode;

	static int simulation_time;
	static int warmup_time;

	static int max_n_rx_packets;

	static int dim_x;
	static int dim_y;

	static int random_seed;

	static bool is_vcd_traced;
	static string vcd_filename;

	static int platform_type;

	static int cmos_process;

	static double input_voltage;	// voltage (V)

	static int clk_freq_mode;	// fixed or maximum
	static double input_clk_freq;	// clk rate (MHz)
	static double operating_clk_freq;	// real operating clk freq
};

#endif /* COMMON_PARAMETER_H_ */
