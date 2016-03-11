/*
 * buffer.h
 *
 *  Created on: Apr 13, 2010
 *      Author: Anh Tran, VCL, UC Davis
 *
 *  Model of buffer using C++ queue class with full/empty signal
 *  Read/write at positive clock edges
 *
 *	Modified:
 *		+ 2010.10.25: add initialize() function for setup buffer size and reserve space
 *		+ 2010.11.18: don't send full signal; used for credit-based flow control
 */

#ifndef BUFFER_SAVE_H_
#define BUFFER_SAVE_H_

#include <queue>
#include <systemc.h>
#include "../../definition.h"
#include "../router_parameters.h"
#include "../router_activities.h"

using namespace std;

SC_MODULE(Buffer){

public:

	void initilize(unsigned int _buffer_size);

	// reset and clock
	sc_in <bool> clk;
	sc_in <bool> reset;

	// write side
	sc_in <bool> valid_in;
	sc_in <Flit> buffer_in;

	// read side
	sc_in <bool> out_buffer_rd;
	sc_out <Flit> buffer_out;
	sc_out <bool> empty;

	// for energy estimation
	BufferActivities *getBufferActivities();

	SC_CTOR(Buffer){
		SC_METHOD(buffer_process);
		sensitive << reset.pos() << clk.pos();
	}

private:

	queue <Flit> buffer;	// a C++ built-in queue
	unsigned int buffer_size;	// number of flits

	// process
	void buffer_process();	// write

	// for energy and power computation
	int n_rd_wr_cycles;
	int n_rd_only_cycles;
	int n_wr_only_cycles;
	int n_inactive_cycles;
};

#endif /* BUFFER_H_ */
