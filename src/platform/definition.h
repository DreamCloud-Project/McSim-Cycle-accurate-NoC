/*
 * definition.h
 *
 *  Created on: Apr 12, 2010
 *      Author: Anh Tran, VCL, UC Davis
 *
 *  Definition of Flit and Packet
 */

#ifndef DEFINITION_H_
#define DEFINITION_H_

#include <systemc.h>
#include <iomanip>
#include "common_parameters.h"
#include "router/router_parameters.h"

using namespace std;

struct Flit {
	bool head;
	bool tail;
	int src_x;
	int src_y;
	int dst_x;
	int dst_y;
	int payload;
	int injected_time;
	int fifo_id;
	int vc_id;	// dynamically change at its virtual-channel router
	int current_out_port;	// used by local shared buffers
	int next_out_port;	// lookahead routing
	int type;	// ACK(=1) or data(=0), using for adaptive in-order routing
	bool ACK_required;	// 1= YES, 0=NO
	int packet_length;// contained in the head flit for packet-length-aware adaptive routing
	int readRequestId; // -1 for write request
	int readRequestSize;
	bool readResponse;
	int packetId;
	int priority;
	int flitId;

	Flit() {
		head = 0;
		tail = 0;
		src_x = 0;
		src_y = 0;
		dst_x = 0;
		dst_y = 0;
		payload = 0;
		injected_time = -1;
		fifo_id = -1;
		vc_id = -1;
		current_out_port = -1;
		next_out_port = -1;
		type = 0;
		ACK_required = 0;
		packet_length = 1;
		readRequestId = -1;
		readRequestSize = -1;
		readResponse = false;
		packetId = -1;
		priority = -1;
		flitId = -1;
	}

	inline bool operator ==(const Flit &flit) const {
		return flit.head == head
				&& flit.tail == tail
				&& flit.src_x == src_x
				&& flit.src_y == src_y
				&& flit.dst_x == dst_x
				&& flit.dst_y == dst_y
				&& flit.payload == payload
				&& flit.injected_time == injected_time
				&& flit.fifo_id == fifo_id
				&& flit.vc_id == vc_id
				&& flit.current_out_port == current_out_port
				&& flit.next_out_port == next_out_port
				&& flit.type == type
				&& flit.ACK_required == ACK_required
				&& flit.packet_length == packet_length
				&& flit.readRequestId == readRequestId
				&& flit.priority == priority
				&& flit.packetId == packetId
				&& flit.flitId == flitId;
	}

	inline bool operator <(const Flit &flit) const {
		return (priority < flit.priority || packetId > flit.packetId
				|| flitId > flit.flitId);
	}
};

class Packet {
public:
	Flit *flit[MAX_PACKET_LENGTH];

	Packet() {
	}

	Packet(Flit *head_flit, int packet_length);

private:
};

// Output overloading for avoiding os errors while compiled
inline ostream & operator <<(ostream & os, const Flit & flit) {
	os << "[";
	if (flit.head) {
		os << "HEAD";
	} else if (!flit.tail) {
		os << "BODY";
	} else {
		os << "TAIL";
	}
	os << ", flit_id: " << flit.flitId;
	os << ", pkt_id: " << std::right << std::setw(4) << flit.packetId;
	os << ", pkt_flits_cnt: " << std::right << std::setw(2)
			<< flit.packet_length;
	os << ", src: (" << flit.src_x << "," << flit.src_y << ")";
	os << ", dst: (" << flit.dst_x << "," << flit.dst_y << ")";
	os << ", inj_time: " << flit.injected_time;
	os << ", read_req_id: " << flit.readRequestId;
	os << ", read_req_size: " << flit.readRequestSize;
	os << ", read_answ: " << (flit.readResponse ? "true" : "false");
	os << ", prio: " << flit.priority;
	os << ", vc: " << flit.vc_id;
	os << "]";

	return os;
}

// Trace overloading for avoiding sc_trace errors while compiled
inline void sc_trace(sc_trace_file * &tf, const Flit &flit, string &name) {
	sc_trace(tf, flit.packetId, name + ".packetId");
	sc_trace(tf, flit.head, name + ".head");
	sc_trace(tf, flit.tail, name + ".tail");
	sc_trace(tf, flit.flitId, name + ".flitId");
}

#endif /* DEFINITION_H_ */
