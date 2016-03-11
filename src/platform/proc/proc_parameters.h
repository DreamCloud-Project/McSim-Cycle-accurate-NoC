/*
 * ProcessorParameters.h
 *
 *  Created on: Aug 18, 2011
 *      Author: anhttran
 */

#ifndef PROCESSORPARAMETERS_H_
#define PROCESSORPARAMETERS_H_

/*
#define TRAFFIC_RANDOM 1
#define TRAFFIC_TRANSPOSE 2
#define TRAFFIC_BIT_COMPLEMENT 3
#define TRAFFIC_BIT_REVERSAL 4
#define TRAFFIC_TORNADO 5
#define TRAFFIC_NEIGHBOR 6	// 80% packets are sent to 4 neighbors, 20% random
#define TRAFFIC_REGIONAL 7	// 80% of packets' destinations are in 3-hop away from its' sources
#define TRAFFIC_HOTSPOT 8

#define REGIONAL_PERCENTAGE 80
#define HOSPOT_PERCENTAGE 30
*/

#define FLIT_INJECTION_RATE 0.1
//#define MAX_SOURCE_QUEUE_LENGTH 1024	// source_queue model at each source PE

#define PACKET_LENGTH_TYPE_FIXED 0
#define PACKET_LENGTH_TYPE_VARIABLE 1

#define PACKET_LENGTH 5	// <= MAX_PACKET_LENGTH, used for fixed packet length


#define INTER_ARRIVAL_IDENTICAL 0	// inter-arrival time is identical
#define INTER_ARRIVAL_EXPONENTIAL 1	// inter-arrival time has exponential distribution

#define DELIVERY_WITHOUT_ACK 1
#define DELIVERY_WITH_ACK 2

#define SENDING_ACK_ALWAYS 1
#define SENDING_ACK_OPTIMAL 2

struct ProcessorParameters{	// using for both Synthetic and Embedded traffics
//	static int traffic_type;

	static double flit_inject_rate;

	static int packet_length_type;

	static int packet_length;

	static int min_packet_length;

	static int max_packet_length;

	static int inter_arrival_time_type;

	static int packet_delivery_type;	// with or without ACK

	static int sending_ack_type;	// always or optimal (based on the distance)
};


#endif /* PROCESSORPARAMETERS_H_ */
