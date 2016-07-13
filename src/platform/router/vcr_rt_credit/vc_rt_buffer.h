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
#include "../../MemoryProfiled.h"
#include "../../proc/dreamcloud/dc_proc.h"

using namespace std;

#ifdef MEM_PROF
class VCRTBuffer : public sc_module, MemoryProfiled<VCRTBuffer> {
#else
class VCRTBuffer: public sc_module {
#endif

private:

	// process
	void buffer_method();

	// utility functions
	void handleRead();
	void handleWrite();

	queue<Flit> buffer;	// a C++ built-in queue
	unsigned int buffer_size;	// number of flits
	unsigned int max_buffer_size;	// max number of flits

public:
	// reset and clock
	sc_in<bool> clk;
	sc_in<bool> reset;

	// write side
	sc_in<bool> valid_in;
	sc_in<Flit> buffer_in;
	sc_out<bool> full;

	// read side
	sc_in<bool> rd_req;
	sc_out<Flit> buffer_out;
	sc_out<bool> empty;

	int port_id; // the input port id
	int vc_id; // the VC id in the input port

	SC_HAS_PROCESS(VCRTBuffer);
	VCRTBuffer(sc_module_name name_) :
			sc_module(name_) {
		SC_METHOD(buffer_method);
		sensitive << reset.pos() << clk.pos();
	}

	// set buffer_size, buffer_reserve
	void initilize(unsigned int _buffer_size);
};

#endif /* VC_BUFFER_H_ */
