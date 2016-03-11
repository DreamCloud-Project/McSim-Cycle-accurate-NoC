/*
 * global_varibles.h
 *
 *  Created on: Jan 21, 2011
 *      Author: Anh Tran
 *
 * Global variables used for simulation are declared here
 */

#ifndef GLOBAL_VARIABLES_H_
#define GLOBAL_VARIABLES_H_

#include <fstream>

using namespace std;

struct GlobalVariables{
	static int n_total_rx_packets;
	static int last_simulation_time;
};

#endif /* GLOBAL_VARIABLES_H_ */
