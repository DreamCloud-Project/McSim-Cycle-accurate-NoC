/*
 * in_port_state_update.h
 *
 *  Created on: Nov 22, 2010
 *      Author: Anh Tran
 *
 *  Input port states
 *
 */

#ifndef IN_PORT_STATE_UPDATE_H_
#define IN_PORT_STATE_UPDATE_H_

#include "systemc.h"
#include "../../router_parameters.h"

SC_MODULE (InPortStateUpdate) {
	int in_port;	// input port identification

	sc_in<bool> empty;
	sc_in<bool> tail_out_reg[N_ROUTER_PORTS];
	sc_in<int> sa_allocated_reg;// = [po]: which output port has been reserved for this input buffer
	sc_in<int> input_port_state_reg; // input port state

	sc_out<int> input_port_state;

	void in_port_state_method() {

		switch (input_port_state_reg.read()) {

		case (IDLE):
			if (!empty.read())	// having at least a flit
				input_port_state.write(WAIT_FOR_GRANT);	// assumed finishing RC
			else
				input_port_state.write(IDLE);
			break;

		case (WAIT_FOR_GRANT):

			// If an output port has been allocated
			if (sa_allocated_reg.read() >= 0) {

				// If the tail of the current packet left
				// then me move back to wait or idle state
				int po = sa_allocated_reg.read();
				if (tail_out_reg[po].read()) {
					if (!empty.read()) {
						input_port_state.write(WAIT_FOR_GRANT);
					}
					else {
						input_port_state.write(IDLE);
					}
				}

				// The input_port_state signal contains the ID of
				// the output state allocated for the current packet
				else {
					input_port_state.write(po);
				}
			}

			// Else, we still need to wait
			else {
				input_port_state.write(WAIT_FOR_GRANT);
			}
			break;

		default: // po >=0: if was already allocated

			// If the tail of the current packet left
			// then me move back to wait or idle state
			int po = input_port_state_reg.read();
			if (tail_out_reg[po].read()) {
				if (!empty.read()) {
					input_port_state.write(WAIT_FOR_GRANT);
				} else {
					input_port_state.write(IDLE);
				}
			}

			// The input_port_state signal contains the ID of
			// the output state allocated for the current packet
			else {
				input_port_state.write(po);
			}
			break;
		}
	}

	SC_CTOR (InPortStateUpdate) {
		SC_METHOD(in_port_state_method);
		sensitive << empty << sa_allocated_reg;
		for (int po = 0; po < N_ROUTER_PORTS; po++) {
			sensitive << tail_out_reg[po];
		}
		sensitive << input_port_state_reg;
	}
};

#endif /* IN_PORT_STATE_UPDATE_H_ */
