/*
 * sa_allocator.cpp
 *
 *  Created on: Jan 13, 2011
 *      Author: anhttran
 */

#include "sa_allocator.h"

/*
 * adjust req signals
 */
void SwitchAllocator::sa_req_adjusted_process(){
	for (int pi=0; pi<N_ROUTER_PORTS; pi++){
		if (in_port_state[pi].read() == WAIT_FOR_GRANT)
			sa_req_adjusted[pi].write(in_port_out_port_req[pi].read());
		else
			sa_req_adjusted[pi].write(NOT_REQ);
	}
}

/*
 * update priorities
 * only round-robin because the SA has only one stage
 */
void SwitchAllocator::sa_priority_new_process(){
	for (int po=0; po<N_ROUTER_PORTS; po++){
		if (sa_grant[po].read() >= 0){		// granted
			int tmp = sa_grant[po].read() + 1;
			sa_priority_new[po].write(tmp % (N_ROUTER_PORTS));	// modulo of num of input ports
		}
		else{
			sa_priority_new[po].write(sa_priority_reg[po].read());	// keep old priority
		}
	}
}
