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

#ifndef VC_RT_BUFFER_H_
#define VC_RT_BUFFER_H_

#include <queue>
#include <systemc.h>
//#include "parameters.h"
#include "../../definition.h"
#include "../router_parameters.h"

using namespace std;

SC_MODULE(VCRTBuffer){

	// reset and clock
	sc_in <bool> clk;
	sc_in <bool> reset;

	// write side
	sc_in <bool> valid_in;
	sc_in <Flit> buffer_in;
	sc_out <bool> full;

	// read side
	sc_in <bool> rd_req;
	sc_out <Flit> buffer_out;
	sc_out <bool> empty;

	int port_id; // the input port id
	int vc_id; // the VC id in the input port

	// process
	void buffer_process();	// write

	SC_CTOR(VCRTBuffer){
		SC_METHOD(buffer_process);
		sensitive << reset.pos() << clk.pos();

	}

public:
	// set buffer_size, buffer_reserve
	void initilize(unsigned int _buffer_size);

//	void initilize(unsigned int _buffer_size, int _buffer_th_off);

//	void initilize(unsigned int _buffer_size, int _buffer_th_off, int _buffer_th_on);

	unsigned int buffer_size;	// number of flits
//	int buffer_th_off;	// assert full when the idle number of entries <= Toff
//	int buffer_th_on;	// assert full when the idle number of entries >= Ton

	// functions
private:
	queue <Flit> buffer;	// a C++ built-in queue

//	bool is_full();	// whether the buffer is full

//	bool is_off();
};

#endif /* VC_BUFFER_H_ */
