/*
 * embedded_proc.cpp
 *
 *  Created on: Aug 18, 2011
 *      Author: anhttran
 */

#include "embedded_proc.h"

/*
 * Initialize this proc
 */
void EmbeddedProc::initialize(int x, int y) {

	local_x = x;
	local_y = y;
	TileLocation local_loc = TileLocation(x, y);
	int local_id = tile_loc_to_id(local_loc, CommonParameter::dim_x,
			CommonParameter::dim_y);
	if (EmbeddedParameters::app_info.count(local_id) > 0) {
		n_dsts = EmbeddedParameters::app_info.find(local_id)->second.n_dsts;
	} else {
		n_dsts = 0;
	}

	for (int i = 0; i < n_dsts; i++) {
		int dst_tile_id =
				EmbeddedParameters::app_info.find(local_id)->second.dst_info[i].dst_tile_id;

		dst_tile_loc[i] = tile_id_to_loc(dst_tile_id, CommonParameter::dim_x,
				CommonParameter::dim_y);

		double req_bandwidth =
				EmbeddedParameters::app_info.find(local_id)->second.dst_info[i].bandwidth;
		flit_inj_rate[i] = (req_bandwidth
				/ EmbeddedParameters::max_req_bandwidth)
				* ProcessorParameters::flit_inject_rate;
	}
}

/*
 *
 */
ProcEvaluationFactors *EmbeddedProc::evaluation() {
	EmbeddedFactors *embedded_factors = new EmbeddedFactors();

	embedded_factors->n_injected_packets = injected_packet_count;
	embedded_factors->n_received_packets = received_packet_count;
	embedded_factors->total_latency = total_latency;
	embedded_factors->max_latency = max_latency;
	embedded_factors->min_latency = min_latency;

	return embedded_factors;
}

/*
 * flit transmit process
 */
void EmbeddedProc::tx_method() {
	if (reset.read()) {	// if reset
		injected_packet_count = 0;
		for (int i = 0; i < n_dsts; i++) { // scan all destinations
			next_injection_time[i] = inter_injection_time(flit_inj_rate[i],
					ProcessorParameters::inter_arrival_time_type);
		}

		// flush all data in the source_queue
		while (!source_queue.empty()) {
			source_queue.pop();
		}

		queue_out_valid.write(0);
		queue_out.write(Flit());

		for (int vo = 0; vo < RouterParameter::n_VCs; vo++) {
			count_minus[vo].write(0);
		}
	} else {	// positive clock edge

		// send flits to its local router
		if (out_vc_remain[0].read() >= 1 && (source_queue.size() > 0)) {// if local port of router is not full
			count_minus[0].write(true);
			source_queue.pop();
		} else {
			count_minus[0].write(false);
		}

		// scan all destinations to generate packets
		int current_time = (int) (sc_time_stamp().to_double() / 1000);
		for (int i = 0; i < n_dsts; i++) {
			if (current_time >= next_injection_time[i]) {
				// create a new packet;
				Flit *head_flit = NULL;

				head_flit = create_head_flit_fixed_dest(local_x, local_y,
						dst_tile_loc[i].x, dst_tile_loc[i].y, current_time);

				int current_packet_length;
				if (ProcessorParameters::packet_length_type
						== PACKET_LENGTH_TYPE_FIXED)
					current_packet_length = ProcessorParameters::packet_length;
				else
					// generate a random packet length in range [min.max]
					current_packet_length =
							rand()
									% (ProcessorParameters::max_packet_length
											- ProcessorParameters::min_packet_length
											+ 1)
									+ ProcessorParameters::min_packet_length;

				Packet *packet = new Packet(head_flit, current_packet_length);
				injected_packet_count += 1;

				// push all its flits to the source_queue
				for (int j = 0; j < current_packet_length; j++) {
					source_queue.push(*(packet->flit[j]));// push value of flit to queue, not pointer
				}
				delete packet;

				// schedule for next packet injection
				int temp = inter_injection_time(flit_inj_rate[i],
						ProcessorParameters::inter_arrival_time_type);

				if (temp == 0)
					temp = 1;
				next_injection_time[i] = current_time + temp;

				delete (head_flit);
			}
		}

		// send one flit to its local router
		if (source_queue.size() > 0) {
			queue_out_valid.write(true);
			Flit flit_tmp = source_queue.front();
			flit_tmp.vc_id = 0;
			queue_out.write(flit_tmp);
		} else {
			queue_out_valid.write(false);
			queue_out.write(Flit());
		}
	}
}

/*
 * flit_out
 */
void EmbeddedProc::flit_out_method() {
	if (out_vc_remain[0].read() >= 1) {
		valid_out.write(queue_out_valid.read());
		flit_out.write(queue_out.read());
	} else {
		valid_out.write(0);
		flit_out.write(Flit());
	}
}

/*
 * flit receive process
 */
void EmbeddedProc::rx_method() {
	if (reset.read()) {
//		in_full.write(0);	//never full
		for (int vi = 0; vi < RouterParameter::n_VCs; vi++) {
			in_vc_buffer_rd[vi].write(0);
//			in_port_EFC_en.write(1);	// is always enable to accept flits
			was_head[vi] = 0;
		}
		received_packet_count = 0;
		total_latency = 0;
		min_latency = INT_MAX;
		max_latency = 0;

	} else {	// if positive clk
		if (valid_in.read()) {
			int vc_id = flit_in.read().vc_id;
			in_vc_buffer_rd[vc_id].write(1);// always read if having flit coming

			for (int vi = 0; vi < RouterParameter::n_VCs; vi++) {
				if (vi != vc_id)
					in_vc_buffer_rd[vi].write(0);
			}

			int current_time = (int) (sc_time_stamp().to_double() / 1000);
			if (current_time >= CommonParameter::warmup_time) {

				Flit rx_flit = flit_in.read();
				if (rx_flit.head) {
//					received_packet_count += 1;
					head_injected_time = rx_flit.injected_time;
					was_head[vc_id] = 1;
				}
				if (rx_flit.tail) {
					if (was_head[vc_id]) {
						received_packet_count += 1;

						int tail_receiving_time = current_time;

						int packet_latency = tail_receiving_time
								- head_injected_time;

						if (packet_latency < 0) {
							cout
									<< "WARNING as a severe ERROR: packet latency is NEGATIVE at Proc of "
									<< this->get_parent()->basename() << "; = "
									<< packet_latency << "; at cycle = "
									<< current_time << endl;
							exit(0);
						}

						total_latency += packet_latency;

						if (packet_latency > max_latency)
							max_latency = packet_latency;

						if (packet_latency < min_latency)
							min_latency = packet_latency;

						was_head[vc_id] = 0;

						if (CommonParameter::sim_mode == SIM_MODE_PACKET) {
							GlobalVariables::n_total_rx_packets += 1;

							if (GlobalVariables::n_total_rx_packets
									>= CommonParameter::max_n_rx_packets) {
								GlobalVariables::last_simulation_time =
										current_time;
								sc_stop();	// stop simulation
							}
						}
					} else {
						// no thing
					}
				}
			} else {
				// nothing
			}
		} else {
			for (int vi = 0; vi < RouterParameter::n_VCs; vi++) {
				in_vc_buffer_rd[vi].write(0);
			}
		}
	}
}

/*
 * update out_vc_remain of all VCs
 */
void EmbeddedProc::out_vc_remain_method() {
	for (int vo = 0; vo < RouterParameter::n_VCs; vo++) {
		int tmp = out_vc_remain_reg[vo];
		if (count_minus[vo].read())
			tmp -= 1;
		if (count_plus[vo].read())
			tmp += 1;
		out_vc_remain[vo].write(tmp);
	}
}

/*
 * pipelined out_vc_remain
 */
void EmbeddedProc::out_vc_remain_reg_method() {
	if (reset.read()) {
		for (int vo = 0; vo < RouterParameter::n_VCs; vo++) {
			out_vc_remain_reg[vo].write((int) RouterParameter::buffer_size);
		}
	} else {
		for (int vo = 0; vo < RouterParameter::n_VCs; vo++) {
			out_vc_remain_reg[vo].write(out_vc_remain[vo].read());
		}
	}
}

/*
 * count_plus = out_vc_buffer_rd
 */
void EmbeddedProc::count_plus_method() {
	for (int vo = 0; vo < RouterParameter::n_VCs; vo++) {
		count_plus[vo].write(out_vc_buffer_rd[vo].read());
	}
}
