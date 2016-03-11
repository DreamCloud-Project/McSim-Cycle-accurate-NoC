/*
 * routing.cpp
 *
 *  Created on: Oct 19, 2010
 *      Author: anhttran
 *  Modified:
 *  	- adaptive routing
 *  	- adaptive routing with packet length awareness
 */

#include "routing_algorithms.h"

// common routing algorithm
int routing(int _src_x, int _src_y, int _local_x, int _local_y, int _dst_x,
		int _dst_y, int out_port_state[N_ROUTER_PORTS],
		int out_port_credit[N_ROUTER_PORTS], int packet_length) {

	switch (RouterParameter::routing_type) {
	case (ROUTING_XY):	// if using XY routing algorithm
		return routing_xy(_local_x, _local_y, _dst_x, _dst_y);
		break;

	case (ROUTING_SOURCE):	// if using source-routing algorithm
		break;

	case (ROUTING_MINIMAL_WEST_FIRST):// if using adaptive west-first turn model
		return routing_minimal_west_first(_local_x, _local_y, _dst_x, _dst_y,
				out_port_state, out_port_credit, packet_length);
		break;

	case (ROUTING_MINIMAL_NEGATIVE_FIRST): // if using adaptive negative-first turn model
		return routing_minimal_negative_first(_local_x, _local_y, _dst_x,
				_dst_y, out_port_state, out_port_credit, packet_length);
		break;

	case (ROUTING_MINIMAL_ODD_EVEN):
		return routing_minimal_odd_even(_src_x, _src_y, _local_x, _local_y,
				_dst_x, _dst_y, out_port_state, out_port_credit, packet_length);
		break;
		/*
		 case (ROUTING_MINIMAL_WEST_FIRST_PACKET_LENGTH_AWARE): // packet-length awareness
		 return routing_minimal_west_first_packet_length_aware(_local_x, _local_y, _dst_x, _dst_y, out_port_state, out_port_credit, packet_length);
		 break;

		 case (ROUTING_MINIMAL_NEGATIVE_FIRST_PACKET_LENGTH_AWARE): // if using adaptive negative-first turn model
		 return routing_minimal_negative_first_packet_length_aware(_local_x, _local_y, _dst_x, _dst_y, out_port_state, out_port_credit, packet_length);
		 break;

		 case (ROUTING_MINIMAL_ODD_EVEN_PACKET_LENGTH_AWARE):
		 return routing_minimal_odd_even_packet_length_aware(_src_x, _src_y, _local_x, _local_y, _dst_x, _dst_y, out_port_state, out_port_credit, packet_length);
		 break;
		 */
	default:
		;
	}

	return 0;
}

// XY routing algorithm
int routing_xy(int _local_x, int _local_y, int _dst_x, int _dst_y) {
	if (_dst_x == _local_x) {
		if (_dst_y == _local_y)
			return LOCAL;
		else if (_dst_y < _local_y)
			return NORTH;
		else
			return SOUTH;
	} else if (_dst_x < _local_x)
		return WEST;
	else
		return EAST;
}

/* profitable (minimal) adaptive routing algorithm: West-First
 * refer X first if both X and Y ports are okie --> output is only one port --> simple SA
 * if multiple outputs are possible, choose the output with lower dimension
 */
int routing_minimal_west_first(int _local_x, int _local_y, int _dst_x,
		int _dst_y, int out_port_state[N_ROUTER_PORTS],
		int out_port_credit[N_ROUTER_PORTS], int packet_length) {
	if (_dst_x == _local_x) {
		if (_dst_y == _local_y)
			return LOCAL;
		else if (_dst_y < _local_y)
			return NORTH;
		else
			return SOUTH;
	} else if (_dst_x < _local_x)
		return WEST;
	else {
		if (_dst_y == _local_y)
			return EAST;
		else if (_dst_y < _local_y) {
			int tmp = -1;

			if ((RouterParameter::packet_length_aware == 0)
					|| ((RouterParameter::packet_length_aware == 1)
							&& (ceil(
									packet_length
											/ RouterParameter::buffer_size)
									>= abs(_dst_x - _local_x)
											+ abs(_dst_y - _local_y)))) {
				// allow adaptive
				switch (RouterParameter::output_channel_select) {
				case (OUTPUT_CHANNEL_SELECT_NEARER_DIMENSION):
					tmp = (nearer_dimension(_local_x, _local_y, _dst_x, _dst_y)
							== X_DIMENSION) ? EAST : NORTH;
					break;
				case (OUTPUT_CHANNEL_SELECT_FARTHER_DIMENSION):
					tmp = (nearer_dimension(_local_x, _local_y, _dst_x, _dst_y)
							== X_DIMENSION) ? NORTH : EAST;
					break;
				case (OUTPUT_CHANNEL_SELECT_XY_ORDERED):
					tmp = EAST;
					break;
				case (OUTPUT_CHANNEL_SELECT_ROUNDROBIN):
					break;
				case (OUTPUT_CHANNEL_SELECT_HIGHER_CREDIT):
					tmp = (out_port_credit[EAST] >= out_port_credit[NORTH]) ?
							EAST : NORTH;
					break;
				default:
					;
				}

				return ((out_port_state[EAST] < 0)
								&& (out_port_state[NORTH] < 0)) ? tmp :
						((out_port_state[EAST] < 0)
								&& (out_port_state[NORTH] >= 0)) ? EAST :
						((out_port_state[EAST] >= 0)
								&& (out_port_state[NORTH] < 0)) ?
								NORTH : NOT_REQ;
			} else {
				// deterministic only
				switch (RouterParameter::output_channel_select) {
				case (OUTPUT_CHANNEL_SELECT_NEARER_DIMENSION):
					tmp = (nearer_dimension(_local_x, _local_y, _dst_x, _dst_y)
							== X_DIMENSION) ? EAST : NORTH;
					break;
				case (OUTPUT_CHANNEL_SELECT_FARTHER_DIMENSION):
					tmp = (nearer_dimension(_local_x, _local_y, _dst_x, _dst_y)
							== X_DIMENSION) ? NORTH : EAST;
					break;
				case (OUTPUT_CHANNEL_SELECT_XY_ORDERED):
					tmp = EAST;
					break;
				default:
					tmp = EAST;
				}

				return tmp;
			}
		} else {
			int tmp = -1;

			if ((RouterParameter::packet_length_aware == 0)
					|| ((RouterParameter::packet_length_aware == 1)
							&& (ceil(
									packet_length
											/ RouterParameter::buffer_size)
									>= abs(_dst_x - _local_x)
											+ abs(_dst_y - _local_y)))) {
				// allow adaptive
				switch (RouterParameter::output_channel_select) {
				case (OUTPUT_CHANNEL_SELECT_NEARER_DIMENSION):
					tmp = (nearer_dimension(_local_x, _local_y, _dst_x, _dst_y)
							== X_DIMENSION) ? EAST : SOUTH;
					break;
				case (OUTPUT_CHANNEL_SELECT_FARTHER_DIMENSION):
					tmp = (nearer_dimension(_local_x, _local_y, _dst_x, _dst_y)
							== X_DIMENSION) ? SOUTH : EAST;
					break;
				case (OUTPUT_CHANNEL_SELECT_XY_ORDERED):
					tmp = EAST;
					break;
				case (OUTPUT_CHANNEL_SELECT_ROUNDROBIN):
					break;
				case (OUTPUT_CHANNEL_SELECT_HIGHER_CREDIT):
					tmp = (out_port_credit[EAST] >= out_port_credit[SOUTH]) ?
							EAST : SOUTH;
					break;
				default:
					;
				}

				return ((out_port_state[EAST] < 0)
								&& (out_port_state[SOUTH] < 0)) ? tmp :
						((out_port_state[EAST] < 0)
								&& (out_port_state[SOUTH] >= 0)) ? EAST :
						((out_port_state[EAST] >= 0)
								&& (out_port_state[SOUTH] < 0)) ?
								SOUTH : NOT_REQ;
			} else {
				// deterministic only
				switch (RouterParameter::output_channel_select) {
				case (OUTPUT_CHANNEL_SELECT_NEARER_DIMENSION):
					tmp = (nearer_dimension(_local_x, _local_y, _dst_x, _dst_y)
							== X_DIMENSION) ? EAST : SOUTH;
					break;
				case (OUTPUT_CHANNEL_SELECT_FARTHER_DIMENSION):
					tmp = (nearer_dimension(_local_x, _local_y, _dst_x, _dst_y)
							== X_DIMENSION) ? SOUTH : EAST;
					break;
				case (OUTPUT_CHANNEL_SELECT_XY_ORDERED):
					tmp = EAST;
					break;
				default:
					tmp = EAST;
				}

				return tmp;
			}
		}

	}
}

/*
 * profitable (minimal) adaptive routing algorithm: Negative-First
 * if multiple outputs are possible, choose the output with lower dimension
 */
int routing_minimal_negative_first(int _local_x, int _local_y, int _dst_x,
		int _dst_y, int out_port_state[N_ROUTER_PORTS],
		int out_port_credit[N_ROUTER_PORTS], int packet_length) {
	if (_dst_x == _local_x) {
		if (_dst_y == _local_y)
			return LOCAL;
		else if (_dst_y < _local_y)
			return NORTH;
		else
			return SOUTH;
	} else if (_dst_x < _local_x) {
		if (_dst_y <= _local_y)
			return WEST;
		else {
			int tmp = -1;

			if ((RouterParameter::packet_length_aware == 0)
					|| ((RouterParameter::packet_length_aware == 1)
							&& (ceil(
									packet_length
											/ RouterParameter::buffer_size)
									>= abs(_dst_x - _local_x)
											+ abs(_dst_y - _local_y)))) {
				// allow adaptive
				switch (RouterParameter::output_channel_select) {
				case (OUTPUT_CHANNEL_SELECT_NEARER_DIMENSION):
					tmp = (nearer_dimension(_local_x, _local_y, _dst_x, _dst_y)
							== X_DIMENSION) ? WEST : SOUTH;
					break;
				case (OUTPUT_CHANNEL_SELECT_FARTHER_DIMENSION):
					tmp = (nearer_dimension(_local_x, _local_y, _dst_x, _dst_y)
							== X_DIMENSION) ? SOUTH : WEST;
					break;
				case (OUTPUT_CHANNEL_SELECT_XY_ORDERED):
					tmp = WEST;
					break;
				case (OUTPUT_CHANNEL_SELECT_ROUNDROBIN):
					break;
				case (OUTPUT_CHANNEL_SELECT_HIGHER_CREDIT):
					tmp = (out_port_credit[WEST] >= out_port_credit[SOUTH]) ?
							WEST : SOUTH;
					break;
				default:
					;
				}

				return ((out_port_state[WEST] < 0)
								&& (out_port_state[SOUTH] < 0)) ? tmp :
						((out_port_state[WEST] < 0)
								&& (out_port_state[SOUTH] >= 0)) ? WEST :
						((out_port_state[WEST] >= 0)
								&& (out_port_state[SOUTH] < 0)) ?
								SOUTH : NOT_REQ;
			} else {
				// deterministic only
				switch (RouterParameter::output_channel_select) {
				case (OUTPUT_CHANNEL_SELECT_NEARER_DIMENSION):
					tmp = (nearer_dimension(_local_x, _local_y, _dst_x, _dst_y)
							== X_DIMENSION) ? WEST : SOUTH;
					break;
				case (OUTPUT_CHANNEL_SELECT_FARTHER_DIMENSION):
					tmp = (nearer_dimension(_local_x, _local_y, _dst_x, _dst_y)
							== X_DIMENSION) ? SOUTH : WEST;
					break;
				case (OUTPUT_CHANNEL_SELECT_XY_ORDERED):
					tmp = WEST;
					break;
				default:
					tmp = WEST;
				}

				return tmp;
			}
		}
	} else {
		if (_dst_y == _local_y)
			return EAST;
		else if (_dst_y < _local_y) {
			int tmp = -1;

			if ((RouterParameter::packet_length_aware == 0)
					|| ((RouterParameter::packet_length_aware == 1)
							&& (ceil(
									packet_length
											/ RouterParameter::buffer_size)
									>= abs(_dst_x - _local_x)
											+ abs(_dst_y - _local_y)))) {
				// allow adaptive
				switch (RouterParameter::output_channel_select) {
				case (OUTPUT_CHANNEL_SELECT_NEARER_DIMENSION):
					tmp = (nearer_dimension(_local_x, _local_y, _dst_x, _dst_y)
							== X_DIMENSION) ? EAST : NORTH;
					break;
				case (OUTPUT_CHANNEL_SELECT_FARTHER_DIMENSION):
					tmp = (nearer_dimension(_local_x, _local_y, _dst_x, _dst_y)
							== X_DIMENSION) ? NORTH : EAST;
					break;
				case (OUTPUT_CHANNEL_SELECT_XY_ORDERED):
					tmp = EAST;
					break;
				case (OUTPUT_CHANNEL_SELECT_ROUNDROBIN):
					break;
				case (OUTPUT_CHANNEL_SELECT_HIGHER_CREDIT):
					tmp = (out_port_credit[EAST] >= out_port_credit[NORTH]) ?
							EAST : NORTH;
					break;
				default:
					;
				}

				return ((out_port_state[EAST] < 0)
								&& (out_port_state[NORTH] < 0)) ? tmp :
						((out_port_state[EAST] < 0)
								&& (out_port_state[NORTH] >= 0)) ? EAST :
						((out_port_state[EAST] >= 0)
								&& (out_port_state[NORTH] < 0)) ?
								NORTH : NOT_REQ;
			} else {
				// deterministic only
				switch (RouterParameter::output_channel_select) {
				case (OUTPUT_CHANNEL_SELECT_NEARER_DIMENSION):
					tmp = (nearer_dimension(_local_x, _local_y, _dst_x, _dst_y)
							== X_DIMENSION) ? EAST : NORTH;
					break;
				case (OUTPUT_CHANNEL_SELECT_FARTHER_DIMENSION):
					tmp = (nearer_dimension(_local_x, _local_y, _dst_x, _dst_y)
							== X_DIMENSION) ? NORTH : EAST;
					break;
				case (OUTPUT_CHANNEL_SELECT_XY_ORDERED):
					tmp = EAST;
					break;
				default:
					tmp = EAST;
				}

				return tmp;
			}
		} else
			return SOUTH;
	}
}

/*
 * minimal odd_even routing
 */
int routing_minimal_odd_even(int _src_x, int _src_y, int _local_x, int _local_y,
		int _dst_x, int _dst_y, int out_port_state[N_ROUTER_PORTS],
		int out_port_credit[N_ROUTER_PORTS], int packet_length) {
	int dist_x = _dst_x - _local_x;	// distance from local to destination
	int dist_y = _dst_y - _local_y;

	OutPortCandidates out_port_candidates;

	if (dist_x == 0) {	// same column with destination
		if (dist_y == 0)
			return LOCAL;
		else if (dist_y > 0)
			return SOUTH;
		else
			return NORTH;
	} else if (dist_x > 0) {	// eastbounce
		if (dist_y == 0)
			return EAST;	// same row with destination
		else {
			if ((_local_x % 2 == 1) || (_local_x == _src_x)) {// odd column or same column with the source
				if (dist_y > 0)
					out_port_candidates.insert(
							OutPortCandidates::value_type(Y_DIMENSION, SOUTH));
				else
					out_port_candidates.insert(
							OutPortCandidates::value_type(Y_DIMENSION, NORTH));
			} else { // even column
					 // don't care
			}

			if ((_dst_x % 2 == 1) || (dist_x >= 2)) {// old destination column or >=2 columns to destination
				out_port_candidates.insert(
						OutPortCandidates::value_type(X_DIMENSION, EAST));
			}
		}
	} else {	// westbounce
		out_port_candidates.insert(
				OutPortCandidates::value_type(X_DIMENSION, WEST));

		if (_local_x % 2 == 0) {	// oven column
			if (dist_y > 0)
				out_port_candidates.insert(
						OutPortCandidates::value_type(Y_DIMENSION, SOUTH));
			else
				out_port_candidates.insert(
						OutPortCandidates::value_type(Y_DIMENSION, NORTH));
		} else {	// odd column
			// don't care
		}
	}

	return out_port_selection(out_port_candidates, _local_x, _local_y, _dst_x,
			_dst_y, out_port_state, out_port_credit, packet_length);
}

/*
 * out_port_selection
 * based on user's parameters
 */
int out_port_selection(OutPortCandidates out_port_candidates, int _local_x,
		int _local_y, int _dst_x, int _dst_y,
		int out_port_state[N_ROUTER_PORTS], int out_port_credit[N_ROUTER_PORTS],
		int packet_length) {
	if (out_port_candidates.size() == 1) { // only one candidate
		if (out_port_candidates.count(X_DIMENSION) > 0)	// on X-dimension
			return out_port_candidates.find(X_DIMENSION)->second;
		else
			return out_port_candidates.find(Y_DIMENSION)->second;
	} else if (out_port_candidates.size() == 2) {	// both dimensions
		int x_port = out_port_candidates.find(X_DIMENSION)->second;
		int y_port = out_port_candidates.find(Y_DIMENSION)->second;

		int tmp = -1;

		// select a port based on user's parameters
		if ((RouterParameter::packet_length_aware == 0)
				|| ((RouterParameter::packet_length_aware == 1)
						&& (ceil(packet_length / RouterParameter::buffer_size)
								>= abs(_dst_x - _local_x)
										+ abs(_dst_y - _local_y)))) {
			// allow adaptive
			switch (RouterParameter::output_channel_select) {
			case (OUTPUT_CHANNEL_SELECT_NEARER_DIMENSION):
				tmp = (nearer_dimension(_local_x, _local_y, _dst_x, _dst_y)
						== X_DIMENSION) ? x_port : y_port;
				break;
			case (OUTPUT_CHANNEL_SELECT_FARTHER_DIMENSION):
				tmp = (nearer_dimension(_local_x, _local_y, _dst_x, _dst_y)
						== X_DIMENSION) ? y_port : x_port;
				break;
			case (OUTPUT_CHANNEL_SELECT_XY_ORDERED):
				tmp = x_port;
				break;
			case (OUTPUT_CHANNEL_SELECT_ROUNDROBIN):
				break;
			case (OUTPUT_CHANNEL_SELECT_HIGHER_CREDIT):
				tmp = (out_port_credit[x_port] >= out_port_credit[y_port]) ?
						x_port : y_port;
				break;
			default:
				;
			}

			return ((out_port_state[x_port] < 0) && (out_port_state[y_port] < 0)) ?
					tmp :
					((out_port_state[x_port] < 0)
							&& (out_port_state[y_port] >= 0)) ? x_port :
					((out_port_state[x_port] >= 0)
							&& (out_port_state[y_port] < 0)) ? y_port : NOT_REQ;
		} else {
			// deterministic only
			switch (RouterParameter::output_channel_select) {
			case (OUTPUT_CHANNEL_SELECT_NEARER_DIMENSION):
				tmp = (nearer_dimension(_local_x, _local_y, _dst_x, _dst_y)
						== X_DIMENSION) ? x_port : y_port;
				break;
			case (OUTPUT_CHANNEL_SELECT_FARTHER_DIMENSION):
				tmp = (nearer_dimension(_local_x, _local_y, _dst_x, _dst_y)
						== X_DIMENSION) ? y_port : x_port;
				break;
			case (OUTPUT_CHANNEL_SELECT_XY_ORDERED):
				tmp = x_port;
				break;
			default:
				tmp = x_port;
			}

			return tmp;
		}
	} else {
		// don't care
		return (-9999);
	}
}

/*
 * the nearer dimension from local to destination (X or Y)
 */
bool nearer_dimension(int _local_x, int _local_y, int _dst_x, int _dst_y) {
	if (abs(_local_x - _dst_x) <= abs(_local_y - _dst_y))
		return X_DIMENSION;
	else
		return Y_DIMENSION;
}
