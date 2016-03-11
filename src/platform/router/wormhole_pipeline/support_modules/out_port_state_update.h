/*
 * out_port_state_update.h
 *
 *  Created on: Jan 12, 2011
 *      Author: anhttran
 */

#ifndef OUT_PORT_STATE_UPDATE_H_
#define OUT_PORT_STATE_UPDATE_H_

#include "systemc.h"
#include "../../router_parameters.h"

SC_MODULE (OutPortStateUpdate){
	int out_port;

	sc_in <int> sa_grant_reg;
	sc_in <bool> tail_out_reg;
	sc_in <int> out_port_state_reg;

	sc_out <int> out_port_state;

	// processes
	void out_port_state_process(){
		switch (out_port_state_reg.read()){
			case (IDLE):
					if ((sa_grant_reg.read()>=0) && !tail_out_reg.read())
						out_port_state.write(sa_grant_reg.read());
					else
						out_port_state.write(IDLE);
					break;
			default:	// >=0
					int pi = out_port_state_reg.read();
					if (tail_out_reg.read())
						out_port_state.write(IDLE);
					else
						out_port_state.write(pi);
				break;
		}
	}

	SC_CTOR (OutPortStateUpdate){
		SC_METHOD (out_port_state_process);
		sensitive << sa_grant_reg << tail_out_reg << out_port_state_reg;
	}

};

#endif /* OUT_PORT_STATE_UPDATE_H_ */
