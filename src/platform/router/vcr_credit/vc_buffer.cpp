/*
 * buffer.cpp
 *
 *  Created on: Apr 13, 2010
 *      Author: Anh Tran, VCL, UC Davis
 *
 *  An implementation of buffer.h
 */


#include <cassert>
#include "vc_buffer.h"

void VCBuffer::initilize(unsigned int _buffer_size){
	buffer_size = _buffer_size;
}


/*
void VCBuffer::initilize(unsigned int _buffer_size, int _buffer_th_off){
	buffer_size = _buffer_size;
	buffer_th_off = _buffer_th_off;
	buffer_th_on = _buffer_th_off + 1;
}

void VCBuffer::initilize(unsigned int _buffer_size, int _buffer_th_off, int _buffer_th_on){
	buffer_size = _buffer_size;
	buffer_th_off = _buffer_th_off;
	buffer_th_on = _buffer_th_on;
}
*/

void VCBuffer::buffer_process(){
	if (reset.read()){	// if reset
		full.write(0);
		empty.write(1);
		// flush all data in the buffer
		while (!buffer.empty()) {
			buffer.pop();
		}

		buffer_out.write(Flit());

//		cout << "RESET..............";
//		cout << "; full = " << isFull();
//		cout << "; empty = " << buffer.empty() << endl;
	}
	else {	// if positive clk edge
		if (valid_in.read() && rd_req.read()){
//			cout << "WRITE and READ.....:";
			// read
			if (buffer.empty())
				cout << "ERROR: read from an EMPTY buffer: "
				<< this->basename() << " of the router at "
				<< this->get_parent()->get_parent()->basename() << endl;

			assert(!buffer.empty());

//			cout << "; payload_removed = " << buffer.front().payload;

			// remove the first flit from the buffer
			buffer.pop();

			// write
//			if (buffer.size() == RouterParameter::buffer_size)
			if (buffer.size() == buffer_size){
				cout << "ERROR: write to a FULL buffer: "
					<< this->basename() << " of the router at "
					<< this->get_parent()->get_parent()->basename() << endl;

			}

//			assert(buffer.size() < RouterParameter::buffer_size);
			assert(buffer.size() < buffer_size);

//			cout << "; payload_in = " << buffer_in.read().payload;
			// write the input flit to the buffer
			buffer.push(buffer_in.read());
//			full.write(is_full());
			empty.write(0);

//			cout << "; full = " << isFull();
//			cout << "; empty = " << buffer.empty();

			buffer_out.write(buffer.front());

//			cout << "; payload_out = " << buffer.front().payload << endl;

		}
		else if (valid_in.read()){

//			cout << "WRITE.....:";
//			if (buffer.size() == RouterParameter::buffer_size){
			if (buffer.size() == buffer_size){
				cout << "ERROR: write to a FULL buffer: "
						<< this->basename() << " of the router at "
						<< this->get_parent()->get_parent()->basename() << endl;
/*
				cout << "The buffer has " << buffer.size()
						<< " entries (in HEX format) are:" << endl;
//				for (unsigned int i=1; i<=RouterParameter::buffer_size; i++){
				for (unsigned int i=1; i<=buffer_size; i++){
					Flit flit = buffer.front();
					cout << hex << uppercase << flit.payload << "; ";
					buffer.pop();
					buffer.push(flit);
				}
				cout << endl;
*/
			}

//			assert(buffer.size() < RouterParameter::buffer_size);
			assert(buffer.size() < buffer_size);

//			cout << "; payload_in = " << buffer_in.read().payload;

			// write the input flit to the buffer
			buffer.push(buffer_in.read());
//			full.write(is_full());
			empty.write(0);

//			cout << "; full = " << isFull();

			buffer_out.write(buffer.front());

//			cout << "; payload_out = " << buffer.front().payload << endl;
		}
		else if (rd_req.read()){
//			cout << "READ.....:";
			if (buffer.empty())
				cout << "ERROR: read from an EMPTY buffer: "
					<< this->basename() << " of the router at "
					<< this->get_parent()->get_parent()->basename() << endl;


			assert(!buffer.empty());

//			cout << "; payload_removed = " << buffer.front().payload;

			// remove the first flit from the buffer
			buffer.pop();
//			full.write(is_full());

			// update flit_out and empty signals
			empty.write(buffer.empty());
			if (!buffer.empty())
				buffer_out.write(buffer.front());
			else
				buffer_out.write(Flit());

//			cout << "; empty = " << buffer.empty();
//			cout << "; payload_out = " << buffer.front().payload << endl;
		}

		full.write(buffer.size() == buffer_size);
	}
}

/*
bool VCBuffer::is_full(){
//	return (buffer.size() >= (RouterParameter::buffer_size - RouterParameter::buffer_reserve));
//	return (buffer.size() >= (buffer_size - buffer_reserve));
	if (buffer_size - buffer.size() <= (unsigned int) buffer_th_off)
		return 1;
	else if (buffer_size - buffer.size() >= (unsigned int) buffer_th_on)
		return 0;
	else
		return (full.read());	// keep old status
}
*/

/*
bool VCBuffer::is_off(){
	if (buffer_size - buffer.size() <= (unsigned int) buffer_th_off)
		return 1;
	else if (buffer_size - buffer.size() >= (unsigned int) buffer_th_on)
		return 0;
	else
		return (full.read());	// keep old status
}
*/


/*
 * For Buffer Testing
 */
// default value for all parameters
//int Parameter::flit_size = FLIT_SIZE;

/*
int Parameter::packet_length = PACKET_LENGTH;
unsigned int Parameter::buffer_size = BUFFER_SIZE;
unsigned int Parameter::buffer_reserve = BUFFER_RESERVE;

int Parameter::dim_x = DIM_X;
int Parameter::dim_y = DIM_Y;

int Parameter::random_seed = time(NULL);
int Parameter::traffic_type = TRAFFIC_RANDOM;

int Parameter::routing_type = ROUTING_XY;
int Parameter::arbiter_type = ARBITER_ROUNDROBIN;
*/

/*
int sc_main(int argc, char *argv[]){
	sc_clock	clk("clk", 1, SC_NS);
	sc_signal <bool> reset;

	sc_signal <bool>	valid_in;
	sc_signal <Flit> buffer_in;
	sc_signal <bool> full;

	sc_signal <bool>	rd_req;
	sc_signal <bool>	empty;
	sc_signal <Flit> buffer_out;

	Buffer	buf("BUFFER");
	buf.clk(clk);
	buf.reset(reset);
	buf.valid_in(valid_in);
	buf.buffer_in(buffer_in);
	buf.full(full);
	buf.rd_req(rd_req);
	buf.empty(empty);
	buf.buffer_out(buffer_out);

	sc_start(0.5, SC_NS);

	sc_trace_file *wf = sc_create_vcd_trace_file("buffer");
	sc_trace(wf, clk, "clock");
	sc_trace(wf, reset, "reset");
	sc_trace(wf, valid_in, "valid_in");
	sc_trace(wf, buffer_in.read().payload, "payload_in");
	sc_trace(wf, full, "full");
	sc_trace(wf, rd_req, "rd_req");
	sc_trace(wf, empty, "empty");
	sc_trace(wf, buffer_out.read().payload, "payload_out");

	reset.write(1);
	valid_in.write(0);
	rd_req.write(0);
	sc_start(2, SC_NS);

	reset.write(0);
	sc_start(1, SC_NS);

	valid_in.write(1);
	Flit *flit_tmp1 = new Flit(1, 0, 1, 1, 2, 2, 11);
	buffer_in.write(*flit_tmp1);
	sc_start(1, SC_NS);
	delete flit_tmp1;

	valid_in.write(1);
	Flit *flit_tmp2 = new Flit(0, 0, 2, 2, 3, 3, 22);
	buffer_in.write(*flit_tmp2);
	sc_start(1, SC_NS);
	delete flit_tmp2;

	valid_in.write(0);
	Flit flit_tmp3(0, 0, 1, 1, 2, 2, 11);
	buffer_in.write(flit_tmp3);
	sc_start(3, SC_NS);

	valid_in.write(1);
	Flit flit_tmp4(0, 0, 3, 3, 4, 4, 33);
	buffer_in.write(flit_tmp4);
	sc_start(1, SC_NS);

	valid_in.write(1);
	Flit flit_tmp5(0, 0, 4, 4, 3, 3, 44);
	buffer_in.write(flit_tmp5);
	sc_start(1, SC_NS);

	valid_in.write(1);
	Flit flit_tmp6(0, 0, 5, 5, 6, 6, 55);
	buffer_in.write(flit_tmp6);
	sc_start(1, SC_NS);

	valid_in.write(0);
	Flit flit_tmp7(0, 0, 1, 1, 2, 2, 11);
	buffer_in.write(flit_tmp7);
	sc_start(1, SC_NS);

	rd_req.write(1);
	sc_start(1, SC_NS);

	rd_req.write(0);
	sc_start(2, SC_NS);

	rd_req.write(1);
	sc_start(1, SC_NS);

	rd_req.write(0);
	sc_start(1, SC_NS);

	valid_in.write(1);
	rd_req.write(1);
	Flit flit_tmp8(0, 0, 6, 6, 5, 5, 66);
	buffer_in.write(flit_tmp8);
	sc_start(1, SC_NS);

	valid_in.write(0);
	rd_req.write(0);
	sc_start(3, SC_NS);

	rd_req.write(1);
	sc_start(1, SC_NS);

	rd_req.write(1);
	sc_start(1, SC_NS);

	rd_req.write(1);
	sc_start(1, SC_NS);

	rd_req.write(1);
	sc_start(1, SC_NS);

	sc_close_vcd_trace_file(wf);
	return 0;
}

*/
