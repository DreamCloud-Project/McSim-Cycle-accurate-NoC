/*
 * definition.cpp
 *
 *  Created on: Apr 12, 2010
 *      Author: Anh Tran, VCL, UC Davis
 */

#include "definition.h"

Packet::Packet(Flit *head_flit, int packet_length){
	// head flits
	flit[0] = head_flit;
	flit[0]->packet_length = packet_length;
	flit[0]->flitId = 0;

	if (packet_length > 1){
		int head_flit_inj_time = head_flit->injected_time;

		// create body flits
		for (int i=1; i<packet_length-1; i++){
			flit[i] = new Flit();
			flit[i]->head = false;
			flit[i]->tail = false;
			flit[i]->payload = rand();
			flit[i]->injected_time = head_flit_inj_time + i;
			flit[i]->src_x = head_flit->src_x;
			flit[i]->src_y = head_flit->src_y;
			flit[i]->dst_x = head_flit->dst_x;
			flit[i]->dst_y = head_flit->dst_y;
			flit[i]->vc_id = head_flit->vc_id;
			flit[i]->readResponse = head_flit->readResponse;
			flit[i]->readRequestId = head_flit->readRequestId;
			flit[i]->readRequestSize = head_flit->readRequestSize;
			flit[i]->priority = head_flit->priority;
			flit[i]->packetId = head_flit->packetId;
			flit[i]->flitId = i;
			flit[i]->packet_length = head_flit->packet_length;
			//		flit[i]->vc_id = 0;
		}

		// create tail flit
		flit[packet_length-1] = new Flit();
		flit[packet_length-1]->head = 0;
		flit[packet_length-1]->tail = 1;
		flit[packet_length-1]->payload = rand();
		flit[packet_length-1]->injected_time = head_flit_inj_time + packet_length-1;
		flit[packet_length-1]->src_x = head_flit->src_x;
		flit[packet_length-1]->src_y = head_flit->src_y;
		flit[packet_length-1]->dst_x = head_flit->dst_x;
		flit[packet_length-1]->dst_y = head_flit->dst_y;
		flit[packet_length-1]->vc_id = head_flit->vc_id;
		flit[packet_length-1]->readResponse = head_flit->readResponse;
		flit[packet_length-1]->readRequestId = head_flit->readRequestId;
		flit[packet_length-1]->readRequestSize = head_flit->readRequestSize;
		flit[packet_length-1]->priority = head_flit->priority;
		flit[packet_length-1]->packetId = head_flit->packetId;
		flit[packet_length-1]->flitId = packet_length-1;
		flit[packet_length-1]->packet_length = head_flit->packet_length;
		//	flit[packet_length-1]->vc_id = 0;
	}
	else {	// packet_length == 1 -> head is also tail
		flit[0]->tail = 1;
	}
}
