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

void VCRTBuffer::initilize(unsigned int _buffer_size) {
	buffer_size = _buffer_size;
}

void VCRTBuffer::buffer_process() {

	if (reset.read()) {	// if reset
		full.write(0);
		empty.write(1);
		// flush all data in the buffer
		while (!buffer.empty()) {
			buffer.pop();
		}

		buffer_out.write(Flit());
	} else {	// if positive clk edge

		if (valid_in.read() && rd_req.read()) {

			// read
			if (buffer.empty()) {
				cout << "ERROR: read from an EMPTY buffer: " << this->basename()
						<< " of the router at "
						<< this->get_parent_object()->get_parent_object()->basename()
						<< endl;
			}
			assert(!buffer.empty());
			buffer.pop();

			// write
			// Checks
			if (buffer.size() == buffer_size) {
				cout
						<< "ERROR: write to a FULL buffer in valid_in && rd_req case: "
						<< this->basename() << " of the router at "
						<< this->get_parent_object()->get_parent_object()->basename()
						<< endl;
				exit(-1);
			}
			if (buffer_in.read().vc_id != this->vc_id) {
				cout << "ERROR: flit in wrong VC " << this->basename()
						<< " of the router at "
						<< this->get_parent_object()->get_parent_object()->basename()
						<< endl;
				exit(-1);
			}
			buffer.push(buffer_in.read());
			empty.write(0);
			buffer_out.write(buffer.front());
		}

		else if (valid_in.read()) {

			// TODO follow debug
			if ((this->basename() == "port=L,VC=0")
					&& (this->get_parent_object()->basename()
							== "Virtual_Channel_RT_Router_8_VCs_Tile[0][1]")) {
				cerr << "size = " << buffer.size() << endl;
			}

			// Checks
			if (buffer_in.read().vc_id != this->vc_id) {
				cout << "ERROR: flit in wrong VC " << this->basename() << " of "
						<< this->get_parent_object()->basename() << endl;
				exit(-1);
			}
			if (buffer.size() == buffer_size) {
				cout
						<< "ERROR: write to a FULL buffer in valid_in case at time "
						<< sc_time_stamp() << ": " << this->basename() << " of "
						<< this->get_parent_object()->basename() << endl;
				exit(-1);
			}

			buffer.push(buffer_in.read());
			empty.write(0);
			buffer_out.write(buffer.front());
		}

		else if (rd_req.read()) {
			if (buffer.empty()) {
				cout << "ERROR: read from an EMPTY buffer: " << this->basename()
						<< " of the router at "
						<< this->get_parent_object()->get_parent_object()->basename()
						<< endl;
			}

			assert(!buffer.empty());
			buffer.pop();

			// update flit_out and empty signals
			empty.write(buffer.empty());
			if (!buffer.empty())
				buffer_out.write(buffer.front());
			else
				buffer_out.write(Flit());
		}

		full.write(buffer.size() == buffer_size);
	}
}
