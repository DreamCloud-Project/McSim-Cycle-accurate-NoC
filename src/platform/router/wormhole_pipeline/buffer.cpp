/*
 * buffer.cpp
 *
 *  Created on: Apr 13, 2010
 *      Author: Anh Tran, VCL, UC Davis
 *
 *  An implementation of buffer.h
 */

#include <cassert>

#include "buffer.h"

void Buffer::initilize(unsigned int _buffer_size) {
	buffer_size = _buffer_size;
}

BufferActivities *Buffer::getBufferActivities() {
	BufferActivities *buffer_activities = new BufferActivities();
	buffer_activities->n_rd_wr_cycles = n_rd_wr_cycles;
	buffer_activities->n_rd_only_cycles = n_rd_only_cycles;
	buffer_activities->n_wr_only_cycles = n_wr_only_cycles;
	buffer_activities->n_inactive_cycles = n_inactive_cycles;
	return buffer_activities;
}

void Buffer::buffer_process() {

	if (reset.read()) {	// if reset
		empty.write(1);

		// flush all data in the buffer
		while (!buffer.empty()) {
			buffer.pop();
		}
		buffer_out.write(Flit());

		// for power & energy estimation
		n_rd_wr_cycles = 0;
		n_rd_only_cycles = 0;
		n_wr_only_cycles = 0;
		n_inactive_cycles = 0;

	} else {	// if positive clk edge
		int current_time = (int) (sc_time_stamp().to_double() / 1000);

		if (valid_in.read() && out_buffer_rd.read()) {

			// read
			if (buffer.empty()) {
				cout << "ERROR: read from an EMPTY buffer: " << this->basename()
						<< " of the router at "
						<< this->get_parent_object()->get_parent_object()->basename() << endl;
				exit(-1);
			}

			// remove the first flit from the buffer
			buffer.pop();

			// write
			if (buffer.size() == buffer_size) {
				cerr << "ERROR: write to a FULL buffer: " << this->basename()
						<< " of the router at "
						<< this->get_parent_object()->get_parent_object()->basename() << endl;
				exit(-1);
			}

			// Write the input flit to the buffer
			buffer.push(buffer_in.read());
			empty.write(0);
			buffer_out.write(buffer.front());
			if (current_time >= CommonParameter::warmup_time)
				n_rd_wr_cycles += 1;

		} else if (valid_in.read()) {

			if (buffer.size() == buffer_size) {
				cerr << "ERROR: write to a FULL buffer: " << this->basename()
						<< " of the router at "
						<< this->get_parent_object()->get_parent_object()->basename() << endl;
				exit(-1);
			}

			// write the input flit to the buffer
			buffer.push(buffer_in.read());
			empty.write(0);
			buffer_out.write(buffer.front());
			if (current_time >= CommonParameter::warmup_time)
				n_wr_only_cycles += 1;

		} else if (out_buffer_rd.read()) {
			if (buffer.empty()) {
				cerr << "ERROR: read from an EMPTY buffer: " << this->basename()
						<< " of the router at "
						<< this->get_parent_object()->get_parent_object()->basename()
						<< endl;
				exit(-1);
			}

			// remove the first flit from the buffer
			buffer.pop();

			// update flit_out and empty signals
			empty.write(buffer.empty());
			if (!buffer.empty())
				buffer_out.write(buffer.front());
			else
				buffer_out.write(Flit());

			if (current_time >= CommonParameter::warmup_time)
				n_rd_only_cycles += 1;
		} else {
			if (current_time >= CommonParameter::warmup_time)
				n_inactive_cycles += 1;
		}
	}
}
