/*
 * sa_arbiter.cpp
 *
 *  Created on: Jan 14, 2011
 *      Author: anhttran
 */

#include "sa_arbiter.h"

//============== function implementation
void SA_Arbiter::sa_arbiter_process(){
	if (out_credit_remain.read() >= 1){
//		bool grant_check = 0;
		if (out_port_state.read() >= 0){	// was granted
			sa_grant.write(out_port_state.read());
//			grant_check = 1;
		}

		else {	// if was not granted or tail flit has been sent that
			int check = -1;
			bool done = 0;

			int inputPort=0;
			while ((!done) && (inputPort<N_ROUTER_PORTS)){	// consider all input ports
				if (sa_req_adjusted[inputPort].read() == out_port){	// if input [pi] requests this output port
					if (inputPort >= sa_priority_reg.read()){	// consider priority
						sa_grant.write(inputPort);

						done = 1;
					}
					else {
						if (check == -1) check = inputPort;
					}
				}
				else {
				}

				inputPort = inputPort + 1;
			}

			if (!done){
				if (check != -1){	// existing an input port is granted this output vc
					sa_grant.write(check);
				}
				else {	// there is no request
					sa_grant.write(NOT_GRANTED);
				}
			}
		}
	}
	else{
		sa_grant.write(NOT_GRANTED);
	}
}

/*
 * arbiter activities
 */
void SA_Arbiter::arbiter_activity_process(){
	if (reset.read()){
		n_active_cycles = 0;
		n_hold_cycles = 0;
		n_not_ready_cycles = 0;
		n_inactive_cycles = 0;

		n_output_change_cycles = 0;
		last_sa_grant = NOT_GRANTED;
	}
	else {	// clk.pos()
		for (int pi=0; pi<N_ROUTER_PORTS; pi++){
			last_req[pi] = (sa_req_adjusted[pi].read() == out_port)?1:0;
		}

		last_hold = (out_port_state.read() >= 0)?1:0;
		last_out_port_ready = (out_credit_remain.read()>0)?1:0;

		int current_sa_grant = sa_grant.read();

		// arbiter activities
		int current_time = (int) (sc_time_stamp().to_double()/1000);
		if (current_time >= CommonParameter::warmup_time){
			if (!last_out_port_ready)
				n_not_ready_cycles += 1;
			else if (last_hold)
				n_hold_cycles += 1;
			else {
				bool has_req = 0;
				for (int pi=0; pi<N_ROUTER_PORTS; pi++){
					if (last_req[pi] == 1) has_req=1;
				}

				if (has_req == 1)
					n_active_cycles += 1;
				else
					n_inactive_cycles += 1;
			}

			if (current_sa_grant != last_sa_grant)
				n_output_change_cycles += 1;
		}

		last_sa_grant = current_sa_grant;
	}
}

/*
 * get_arbiter_activities
 */
ArbiterActivities *SA_Arbiter::get_arbiter_activities(){
	ArbiterActivities *arbiter_activities = new ArbiterActivities();

	arbiter_activities->n_active_cycles = n_active_cycles;
	arbiter_activities->n_hold_cycles = n_hold_cycles;
	arbiter_activities->n_not_ready_cycles = n_not_ready_cycles;
	arbiter_activities->n_inactive_cycles = n_inactive_cycles;

	arbiter_activities->n_output_change_cycles = n_output_change_cycles;

	return arbiter_activities;
}
