/*
 * pe.cpp
 *
 * (C) copyright by the VCL laboratory, ECE Department, UC Davis
 *
 *  Please send en email to anhtrandavis@gmail.com to ask for your permission
 *  before using this code;
 *  and cite the paper "..." in your related publications.
 *
 *  Created on: Apr 19, 2010
 *      Author: Anh Tran, VCL, UC Davis
 *
 *  An implementation of synthetic_proc.h
 *
 *  Revised:
 *  	+ 2011.01.25:
 *  		- support variable packet length
 *  	+ 2011.07.28:
 *  		- support ACK signaling for in-order packet delivery
 *
 */

#include "synthetic_with_ACK_proc.h"


void SyntheticWithACKProc::initialize(int x, int y){
	local_x = x;
	local_y = y;
}

//SyntheticFactors *PE::synthetic_evaluation(){
ProcEvaluationFactors *SyntheticWithACKProc::evaluation(){
	SyntheticFactors *synth_factors = new SyntheticFactors();

	synth_factors->n_injected_packets = injected_packet_count;
	synth_factors->n_received_packets = received_packet_count;
	synth_factors->total_latency = total_latency;
	synth_factors->max_latency = max_latency;
	synth_factors->min_latency = min_latency;

	return synth_factors;
}


// flit transmit process
void SyntheticWithACKProc::tx_process(){
	if (reset.read()){	// if reset
//		has_sent_tail_flit.write(0);

		injected_packet_count = 0;
//		next_injection_time = 0;
		next_injection_time = inter_injection_time(ProcessorParameters::flit_inject_rate, ProcessorParameters::inter_arrival_time_type);

		// flush all data in the source_queue
		while (!source_queue.empty()) {
			source_queue.pop();
		}

//		valid_out.write(0);
//		flit_out.write(Flit());
		source_queue_out_valid.write(0);
		source_queue_out.write(Flit());

//		tail_reg.write(0);

//		for (int vo=0; vo<RouterParameter::n_VCs; vo++){
//			count_minus[vo].write(0);
//		}
	}
	else {	// positive clock edge
/*
		if (out_vc_remain[0].read() >=1){
			// prioritize ACK over data flit
			// only allow to send ACK after a whole packet has been sent
			// (avoid interleaving the ACK flit with data flits of a packet)
			if (ack_queue.size()>0 && (tx_state.read()==TX_HAS_SENT_A_PACKET)){
				has_sent_tail_flit.write(0);

				count_minus[0].write(1);
				ack_queue.pop();
			}
			else if (source_queue.size()>0 && (tx_state.read()==TX_OKIE_TO_SEND)){
				Flit flit_tmp = source_queue.front();

				if (flit_tmp.tail && !flit_tmp.type) // is tail
					has_sent_tail_flit.write(1);
				else
					has_sent_tail_flit.write(0);

				count_minus[0].write(1);
				source_queue.pop();
			}
			else {
				has_sent_tail_flit.write(0);
				count_minus[0].write(0);
			}
		}
		else {
			has_sent_tail_flit.write(0);
			count_minus[0].write(0);
		}
*/
		if (tx_mux_sel.read()==1) source_queue.pop();	// remove a flit

//		int nbits = (int) ceil(log((double)CommonParameter::dim_x) / log(2.0));

//		if ((SyntheticParameters::traffic_type == TRAFFIC_TRANSPOSE)
//				&& (local_x == local_y)){
//			// do nothing, not transfer packet to itself
//		}
//		else if ((SyntheticParameters::traffic_type == TRAFFIC_BIT_REVERSAL)
//				&& (bit_reversal(local_x, nbits) == local_x)
//				&& (bit_reversal(local_y, nbits) == local_y)){
//			// do nothing, not transfer packet to itself
//		}
//		else {
			// generate packets
			int current_time = (int) (sc_time_stamp().to_double()/1000);
			if (current_time >= next_injection_time){
				// create a new packet;
				Flit *head_flit = NULL;
				switch (SyntheticParameters::traffic_type){
					case (TRAFFIC_RANDOM):
						head_flit = create_head_flit_random(local_x, local_y, current_time);
						break;
					case (TRAFFIC_TRANSPOSE):
						head_flit = create_head_flit_transpose(local_x, local_y, current_time);
						break;
					case (TRAFFIC_BIT_COMPLEMENT):
						head_flit = create_head_flit_bit_complement(local_x, local_y, current_time);
						break;
					case (TRAFFIC_BIT_REVERSAL):
						head_flit = create_head_flit_bit_reversal(local_x, local_y, current_time);
						break;
					case (TRAFFIC_TORNADO):
						head_flit = create_head_flit_tornado(local_x, local_y, current_time);
						break;
					case (TRAFFIC_NEIGHBOR):
						head_flit = create_head_flit_neighbor(local_x, local_y, current_time);
						break;
					case (TRAFFIC_REGIONAL):
						head_flit = create_head_flit_regional(local_x, local_y, current_time);
						break;
					case (TRAFFIC_HOTSPOT):
						head_flit = create_head_flit_hotspot(local_x, local_y, current_time);
						break;
					case (TRAFFIC_SHUFFLE):
						head_flit = create_head_flit_shuffle(local_x, local_y, current_time);
						break;
					case (TRAFFIC_ROTATE):
						head_flit = create_head_flit_rotate(local_x, local_y, current_time);
						break;

					default:;
				}

				// create and push a packet to the source queue if the destination is different from the source
				if ((head_flit->dst_x != local_x) || (head_flit->dst_y != local_y)){
					int current_packet_length;
					if (ProcessorParameters::packet_length_type == PACKET_LENGTH_TYPE_FIXED)
						current_packet_length = ProcessorParameters::packet_length;
					else	// generate a random packet length in range [min.max]
						current_packet_length = rand()%(ProcessorParameters::max_packet_length-ProcessorParameters::min_packet_length+1) + ProcessorParameters::min_packet_length;

	//				cout << "current_packet_length = " << current_packet_length << endl;

	//				head_flit->packet_length = current_packet_length;	// used for ACK control

					// require the destination to send ACK or not
					if (ProcessorParameters::sending_ack_type == SENDING_ACK_ALWAYS)
						head_flit->ACK_required = 1;
					else { // == SENDING_ACK_OPTIMAL
						if (current_packet_length >= ceil(abs(head_flit->dst_x-head_flit->src_x) + abs(head_flit->dst_y-head_flit->src_y)+1)/2)
							head_flit->ACK_required = 0;
						else
							head_flit->ACK_required = 1;
					}

					Packet *packet = new Packet(head_flit, current_packet_length);
					injected_packet_count += 1;

					//			cout << "@ cycle " << sc_time_stamp().to_double()/1000
					//					<< ": PE (" << local_x << ", " << local_y << ") injects a packet" << endl;

					// push all its flits to the source_queue
					for (int i=0; i<current_packet_length;i++){
						source_queue.push(*(packet->flit[i]));	// push value of flit to queue, not pointer
					}
					delete packet;	// clean the memory
				}
				// schedule for next packet injection
				int temp = inter_injection_time(ProcessorParameters::flit_inject_rate, ProcessorParameters::inter_arrival_time_type);

				//			cout << "temp = " << temp << endl;

				if (temp == 0) temp = 1;
				next_injection_time = current_time + temp;

				delete (head_flit);
			}
			else {
				// do nothing
			}
//		}

		if (source_queue.size()>0){
			source_queue_out_valid.write(1);

			Flit flit_tmp = source_queue.front();
			flit_tmp.vc_id = 0;
			source_queue_out.write(flit_tmp);
		}
		else {
			source_queue_out_valid.write(0);
			source_queue_out.write(Flit());
		}
	}
}


/*
 * update tx_state
 */
void SyntheticWithACKProc::tx_state_process(){
	switch (tx_state_reg.read()){
		case (TX_OKIE_TO_SEND_EITHER_DATA_OR_ACK):
				if (has_sent_head_flit.read()){
					if (has_sent_tail_flit.read())	// 1-flit packet, always requires ACK
						tx_state.write(TX_WAIT_FOR_ACK); // or TX_OKIE_TO_SEND_ACK
					else
						tx_state.write(TX_OKIE_TO_SEND_DATA);
				}
				else
					tx_state.write(TX_OKIE_TO_SEND_EITHER_DATA_OR_ACK);
				break;
		case (TX_OKIE_TO_SEND_DATA):
				if (has_sent_tail_flit.read()){
					if (!is_ACK_required.read() || has_received_ACK.read() || received_ACK.read())
						tx_state.write(TX_OKIE_TO_SEND_EITHER_DATA_OR_ACK);
					else
						tx_state.write(TX_WAIT_FOR_ACK); // or TX_OKIE_TO_SEND_ACK
				}
				else
					tx_state.write(TX_OKIE_TO_SEND_DATA);
				break;
		case (TX_WAIT_FOR_ACK): // also TX_WAIT_FOR_RECEIVING_ACK
				if (has_received_ACK.read())
					tx_state.write(TX_OKIE_TO_SEND_EITHER_DATA_OR_ACK);
				else
					tx_state.write(TX_WAIT_FOR_ACK);
				break;
		default:;
	}
}


/*
 * update tx_state_reg
 */
void SyntheticWithACKProc::tx_state_reg_process(){
	if (reset.read()){
		tx_state_reg.write(TX_OKIE_TO_SEND_EITHER_DATA_OR_ACK);
	}
	else {
		tx_state_reg.write(tx_state.read());
	}
}

/*
 * tx_mux_sel process
 */
void SyntheticWithACKProc::tx_mux_sel_process(){
	if (out_vc_remain[0].read()>0
			&& tx_state==TX_OKIE_TO_SEND_EITHER_DATA_OR_ACK){
		if (ack_queue_out_valid.read())	// prioritize ACK or Data
			tx_mux_sel.write(2);
		else if (source_queue_out_valid.read())
			tx_mux_sel.write(1);
		else tx_mux_sel.write(0);
	}
	else if (out_vc_remain[0].read()>0
			&& source_queue_out_valid.read()
			&& tx_state==TX_OKIE_TO_SEND_DATA)
		tx_mux_sel.write(1);
	else if (out_vc_remain[0].read()>0
			&& ack_queue_out_valid.read()
			&& tx_state==TX_OKIE_TO_SEND_ACK)
		tx_mux_sel.write(2);
	else
		tx_mux_sel.write(0);
}

/*
 * flit_out
 */
void SyntheticWithACKProc::flit_out_process(){
/*
	if (out_vc_remain[0].read()>=1){
		if (ack_queue_out_valid.read()){	// prioritize ACK over data flit
			valid_out.write(1);
			flit_out.write(ack_queue_out.read());
		}
		else {
			if (tx_state.read() == TX_OKIE_TO_SEND){
				valid_out.write(source_queue_out_valid.read());
				flit_out.write(source_queue_out.read());
			}
			else {
				valid_out.write(0);
				flit_out.write(Flit());
			}
		}
	}
	else {
		valid_out.write(0);
		flit_out.write(Flit());
	}
*/
	if (tx_mux_sel.read()==1){
		valid_out.write(1);
		flit_out.write(source_queue_out.read());
	}
	else if (tx_mux_sel.read()==2){
		valid_out.write(1);
		flit_out.write(ack_queue_out.read());
	}
	else {
		valid_out.write(0);
		flit_out.write(Flit());
	}
}


/*
 * has_sent_tail_flit update
 */
void SyntheticWithACKProc::has_sent_tail_flit_process(){
	if (reset.read()){
		has_sent_tail_flit.write(0);
	}
	else {
		Flit data_flit = source_queue_out.read();
		if (tx_mux_sel==01 && data_flit.tail){
			has_sent_tail_flit.write(1);
		}
		else {
			has_sent_tail_flit.write(0);
		}
	}
}

/*
 * has_head_tail_flit update
 */
void SyntheticWithACKProc::has_sent_head_flit_process(){
	if (reset.read()){
		has_sent_head_flit.write(0);

		is_ACK_required.write(0);
	}
	else {
		Flit data_flit = source_queue_out.read();
		if (tx_mux_sel==01 && data_flit.head){
			has_sent_head_flit.write(1);

			is_ACK_required.write(data_flit.ACK_required);
		}
		else {
			has_sent_head_flit.write(0);

			is_ACK_required.write(is_ACK_required.read());	// no update
		}
	}
}


/*
 * flit receive process
 */
void SyntheticWithACKProc::rx_process(){
	if (reset.read()){
//		in_full.write(0);	//never full
		for (int vi=0; vi<RouterParameter::n_VCs; vi++){
			in_vc_buffer_rd[vi].write(0);
//			in_port_EFC_en.write(1);	// is always enable to accept flits
			was_head[vi] = 0;
		}
		received_packet_count = 0;
		total_latency = 0;
		min_latency = INT_MAX;
		max_latency = 0;

		has_received_ACK.write(0);

		// flush all flits in the ack_queue
		while (!ack_queue.empty()) {
			ack_queue.pop();
		}

	}
	else {	// if positive clk
		if (tx_mux_sel == 2) ack_queue.pop(); // remove a ACK flit

		if (valid_in.read()){
			int vc_id = flit_in.read().vc_id;
			in_vc_buffer_rd[vc_id].write(1);	// always read if having flit coming

			for (int vi=0; vi<RouterParameter::n_VCs; vi++){
				if (vi != vc_id)
					in_vc_buffer_rd[vi].write(0);
			}

			Flit rx_flit = flit_in.read();

			if (rx_flit.head && rx_flit.tail && (rx_flit.type==1)){	// ACK flit
				has_received_ACK.write(1);
			}
			else {	// data flit
				has_received_ACK.write(0);

				int current_time = (int) (sc_time_stamp().to_double()/1000);

				// send a ACK flit back to the source
				// if receiving a new data packet
				// and the packet requires to and ACK
				if (rx_flit.head && (rx_flit.type==0) && rx_flit.ACK_required){
					int dst_x = rx_flit.src_x;
					int dst_y = rx_flit.src_y;

					Flit *ack_flit = create_head_flit_ACK(local_x, local_y, dst_x, dst_y, current_time);

					// push this ACK flit to the ack_queue
					ack_queue.push(*ack_flit);

					delete ack_flit;
				}

				if (current_time >= CommonParameter::warmup_time) {
					if (rx_flit.head) {
	//					received_packet_count += 1;
						head_injected_time = rx_flit.injected_time;
						was_head[vc_id] = 1;
					}
					if (rx_flit.tail){
						if (was_head[vc_id]){
							received_packet_count += 1;

							tail_receiving_time = current_time;

							int packet_latency = tail_receiving_time - head_injected_time;

							if (packet_latency < 0)
								cout << "WARNING: packet latency is NEGATIVE at Proc of "
									<< this->get_parent()->basename() << "; = " << packet_latency
									<< "; at cycle = "<< current_time << endl;

							total_latency += packet_latency;

							if (packet_latency > max_latency)
								max_latency = packet_latency;

							if (packet_latency < min_latency)
								min_latency = packet_latency;

							was_head[vc_id] = 0;

							if (CommonParameter::sim_mode == SIM_MODE_PACKET){
								GlobalVariables::n_total_rx_packets += 1;

								if	(GlobalVariables::n_total_rx_packets >= CommonParameter::max_n_rx_packets){
									GlobalVariables::last_simulation_time = current_time;
									sc_stop();	// stop simulation
								}
							}
						}
						else{
							// no thing
						}
					}
				}
				else{
					// nothing
				}
			}
		}
		else{
			for (int vi=0; vi<RouterParameter::n_VCs; vi++){
				in_vc_buffer_rd[vi].write(0);
			}

			has_received_ACK.write(0);
		}

		// ack_queue front
		if (ack_queue.size()>0){
			ack_queue_out_valid.write(1);

			Flit flit_tmp = ack_queue.front();
			flit_tmp.vc_id = 0;
			ack_queue_out.write(flit_tmp);
		}
		else {
			ack_queue_out_valid.write(0);
			ack_queue_out.write(Flit());
		}

	}
}

/*
 * received_ACK process
 */
void SyntheticWithACKProc::received_ACK_process(){
	if (received_ACK_reg.read()) {
		if (has_sent_head_flit.read())
			received_ACK.write(0);
		else
			received_ACK.write(1);
	} else {
		if (has_received_ACK.read())
			received_ACK.write(1);
		else
			received_ACK.write(0);
	}
}

/*
 * received_ACK_reg process
 */
void SyntheticWithACKProc::received_ACK_reg_process(){
	if (reset.read())	// reset, assuming already received ACK --> allow to send a packet if any
		received_ACK_reg.write(1);
	else
		received_ACK_reg.write(received_ACK.read());
}

/*
 * update out_vc_remain of all VCs
 */
void SyntheticWithACKProc::out_vc_remain_process(){
	for (int vo=0; vo<RouterParameter::n_VCs; vo++){
		int tmp = out_vc_remain_reg[vo];
		if (count_minus[vo].read())
			tmp -= 1;
		if (count_plus[vo].read())
			tmp += 1;
		out_vc_remain[vo].write(tmp);

/*
		switch (out_vc_remain_reg[vo].read()){
			case (0):
					if (count_plus[vo].read() && !count_minus[vo].read())
						out_vc_remain[vo].write(1);
					else
						out_vc_remain[vo].write(0);
					break;
			case (1):
					if (count_minus[vo].read() && !count_plus[vo].read())
						out_vc_remain[vo].write(0);
					else
						out_vc_remain[vo].write(1);
					break;
		}
*/
	}
}


/*
 * pipelined out_vc_remain
 */
void SyntheticWithACKProc::out_vc_remain_reg_process(){
	if (reset.read()){
		for (int vo=0; vo<RouterParameter::n_VCs; vo++){
//			out_vc_remain_reg[vo].write(1);
			out_vc_remain_reg[vo].write((int)RouterParameter::buffer_size);
		}
	}
	else {
		for (int vo=0; vo<RouterParameter::n_VCs; vo++){
			out_vc_remain_reg[vo].write(out_vc_remain[vo].read());
		}
	}
}

/*
 * count_plus = out_vc_buffer_rd
 */
void SyntheticWithACKProc::count_plus_process(){
	for (int vo=0; vo<RouterParameter::n_VCs; vo++){
		count_plus[vo].write(out_vc_buffer_rd[vo].read());
	}
}

/*
 * count_minus
 */
void SyntheticWithACKProc::count_minus_process(){
	if (reset.read()){
		for (int vo=0; vo<RouterParameter::n_VCs; vo++){
			count_minus[vo].write(0);
		}

	}
	else{
		if (tx_mux_sel.read()==01 || tx_mux_sel.read()==02)
			count_minus[0].write(1);
		else
			count_minus[0].write(0);
	}
}



/*
 * For Proc Testing
 */

/*
// default value for all parameters
//int Parameter::flit_size = FLIT_SIZE;

int Parameter::packet_length = PACKET_LENGTH;
unsigned int Parameter::buffer_size = BUFFER_SIZE;
unsigned int Parameter::buffer_reserve = BUFFER_RESERVE;

int Parameter::dim_x = DIM_X;
int Parameter::dim_y = DIM_Y;

int Parameter::random_seed = time(NULL);
int Parameter::traffic_type = TRAFFIC_RANDOM;

double Parameter::flit_inject_rate = FLIT_INJECTION_RATE;

int Parameter::routing_type = ROUTING_XY;
int Parameter::arbiter_type = ARBITER_ROUNDROBIN;

int sc_main(int argc, char *argv[]){
	srand(Parameter::random_seed);

	sc_signal <int> local_x;
	sc_signal <int> local_y;

	local_x.write(3);
	local_y.write(4);


	sc_clock	clk("clk", 1, SC_NS);
	sc_signal <bool> reset;

	sc_signal <bool> valid_in;
	sc_signal <Flit> flit_in;
	sc_signal <bool> in_full;

	sc_signal <bool> valid_out;
	sc_signal <Flit> flit_out;
	sc_signal <bool> out_full;

	PE	proc("Processor");
	proc.local_x(local_x);
	proc.local_y(local_y);
	proc.clk(clk);
	proc.reset(reset);
	proc.valid_in(valid_in);
	proc.flit_in(flit_in);
	proc.in_full(in_full);
	proc.valid_out(valid_out);
	proc.flit_out(flit_out);
	proc.out_full(out_full);


	sc_start(0.5, SC_NS);

	sc_trace_file *wf = sc_create_vcd_trace_file("pe");
	sc_trace(wf, clk, "clock");
	sc_trace(wf, reset, "reset");
	sc_trace(wf, valid_in, "valid_in");
	sc_trace(wf, flit_in.read().payload, "payload_in");
	sc_trace(wf, in_full, "in_full");
	sc_trace(wf, valid_out, "valid_out");
	sc_trace(wf, flit_out.read().payload, "payload_out");
	sc_trace(wf, flit_out.read().head, "head_out");
	sc_trace(wf, flit_out.read().dst_x, "dst_x");
	sc_trace(wf, flit_out.read().dst_y, "dst_y");
	sc_trace(wf, flit_out.read().tail, "tail_out");
	sc_trace(wf, out_full, "out_full");

	reset.write(1);
	out_full.write(0);
//	valid_in.write(0);
//	rd_req.write(0);
	sc_start(2, SC_NS);

	reset.write(0);
	sc_start(1, SC_NS);

	sc_start(100000, SC_NS);

	cout << "number of injected packets = " << proc.injected_packet_count << endl;
	cout << "avarage of inter-injected times = "
			<< sc_time_stamp().to_double()/1000/proc.injected_packet_count << endl;

	sc_close_vcd_trace_file(wf);
	return 0;
}

*/
