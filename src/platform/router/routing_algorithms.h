/*
 * routing.h
 *
 *  Created on: Oct 19, 2010
 *      Author: Anh Tran
 *
 *  A seperate file containing all routing algorithms
 *
 *  Modified:
 *  	+ 2011.09.19:
 *  		- support adaptive routing for bufferless router
 *  		- packet-length-aware adaptive routing for guaranteeing in-order packet delivery
 *
 */

#ifndef ROUTING_ALGORITHMS_H_
#define ROUTING_ALGORITHMS_H_

#include <stdlib.h>
#include <math.h>
#include "router_parameters.h"
#include "../common_parameters.h"

//#include <tr1/unordered_map>	// hashtable from Boost tr1 package embedded in GCC4.1.2
#include <map>

using namespace std;

#define X_DIMENSION 0
#define Y_DIMENSION 1
#define Z_DIMENSION 2	// for 3-D chip

// a hashtable of possible output ports returned by the routing algorithm before selected
// = a set of <dimension, port> pairs
// for example: {<X_DIMENSION, WEST>,<Y_DIMENSION, NORTH>}
//typedef std::tr1::unordered_map<int, int> OutPortCandidates;
typedef map<int, int> OutPortCandidates;

int routing(int _src_x, int _src_y, int _local_x, int _local_y, int _dst_x, int _dst_y, int out_port_state[N_ROUTER_PORTS], int out_port_credit[N_ROUTER_PORTS], int packet_length);

int routing_xy(int _local_x, int _local_y, int _dst_x, int _dst_y);

int routing_minimal_west_first(int _local_x, int _local_y, int _dst_x, int _dst_y, int out_port_state[N_ROUTER_PORTS], int out_port_credit[N_ROUTER_PORTS], int packet_length);
int routing_minimal_negative_first(int _local_x, int _local_y, int _dst_x, int _dst_y, int out_port_state[N_ROUTER_PORTS], int out_port_credit[N_ROUTER_PORTS], int packet_length);

// odd-even routing algorithm also needs the source information
int routing_minimal_odd_even(int _src_x, int _src_y, int _local_x, int _local_y, int _dst_x, int _dst_y, int out_port_state[N_ROUTER_PORTS], int out_port_credit[N_ROUTER_PORTS], int packet_length);

// select a output port from a set of candidates
int out_port_selection(OutPortCandidates out_port_candidates, int _local_x, int _local_y, int _dst_x, int _dst_y, int out_port_state[N_ROUTER_PORTS], int out_port_credit[N_ROUTER_PORTS], int packet_length);

bool nearer_dimension(int _local_x, int _local_y, int _dst_x, int _dst_y);

// west-first with packet-length-awareness for nonACK routing
//int routing_minimal_west_first_packet_length_aware(int _local_x, int _local_y, int _dst_x, int _dst_y, int out_port_state[N_ROUTER_PORTS], int out_port_credit[N_ROUTER_PORTS], int packet_length);

// negative-first with packet-length-awareness for nonACK routing
//int routing_minimal_negative_first_packet_length_aware(int _local_x, int _local_y, int _dst_x, int _dst_y, int out_port_state[N_ROUTER_PORTS], int out_port_credit[N_ROUTER_PORTS], int packet_length);

// odd-even routing algorithm with packet-length-awareness for nonACK routing
//int routing_minimal_odd_even_packet_length_aware(int _src_x, int _src_y, int _local_x, int _local_y, int _dst_x, int _dst_y, int out_port_state[N_ROUTER_PORTS], int out_port_credit[N_ROUTER_PORTS], int packet_length);

#endif /* ROUTING_ALGORITHMS_H_ */
