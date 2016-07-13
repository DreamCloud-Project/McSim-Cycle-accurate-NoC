/*
 * buffer.cpp
 *
 *  Created on: Apr 13, 2010
 *      Author: Anh Tran, VCL, UC Davis
 *
 *  An implementation of buffer.h
 */

#include <cassert>
#include "vc_rt_buffer.h"
#include "../../common_functions.h"

void VCRTBuffer::initilize(unsigned int _buffer_size) {
	buffer_size = _buffer_size;
	max_buffer_size = 0;
}

void VCRTBuffer::handleRead() {

	// Checks this buffer is not empty
	if (buffer.empty()) {
		cerr << "ERROR: read from an EMPTY buffer: " << this->basename()
				<< " of the router at "
				<< this->get_parent_object()->get_parent_object()->basename()
				<< endl;
		exit(-1);
	}

	// Pop the flit on this buffer's output
	buffer.pop();
}

void VCRTBuffer::handleWrite() {

	// Checks the input flit is for this buffer
	if (buffer_in.read().vc_id != this->vc_id) {
		cerr << "ERROR: flit in wrong VC " << this->basename()
				<< " of the router at "
				<< this->get_parent_object()->get_parent_object()->basename()
				<< endl;
		exit(-1);
	}

	// Checks this buffer is not yet full
	if (buffer.size() >= buffer_size) {
		cerr << "Reading and writing ";
		cerr << "ERROR: write to a FULL buffer in valid_in && rd_req case: "
				<< this->basename() << " of the router at "
				<< this->get_parent_object()->get_parent_object()->basename()
				<< endl;
		exit(-1);
	}

	// Push the input flit in this buffer
	buffer.push(buffer_in.read());
}

void VCRTBuffer::buffer_method() {

	if (reset.read()) {	// if reset
		full.write(0);
		empty.write(1);
		// flush all data in the buffer
		while (!buffer.empty()) {
			buffer.pop();
		}
		buffer_out.write(Flit());
	} else {	// if positive clk edge

		// Read
		if (rd_req.read()) {
			handleRead();
		}

		// Write
		if (valid_in.read()) {
			handleWrite();
		}

		// Outputs signals
		empty.write(buffer.empty());
		if (!buffer.empty()) {
			buffer_out.write(buffer.front());
		} else {
			buffer_out.write(Flit());
		}
		full.write(buffer.size() == buffer_size);
	}
}
