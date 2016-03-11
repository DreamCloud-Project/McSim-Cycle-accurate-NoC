/*
 * valid_check_update.h
 *
 *  Created on: Jan 11, 2011
 *      Author: anhttran
 *
 *  Check whether flit_in_reg is valid
 *  (due to already sent at last cycle or written a new value at current cycle)
 */

#ifndef VALID_CHECK_UPDATE_H_
#define VALID_CHECK_UPDATE_H_

#include "systemc.h"
#include "../../router_parameters.h"

SC_MODULE (ValidCheckUpdate){
	int in_port;	// input port identification

	sc_in <bool> valid_in_reg;
	sc_in <int> sa_allocated_reg;	// = [po]: which output port has been reserved for this input buffer
	sc_in <bool> valid_check_reg;

	sc_out <bool> valid_check;

	void valid_check_update_process(){
			switch (valid_check_reg.read()){
				case (0):
						if (valid_in_reg.read())	// having a new incoming flit
							valid_check.write(1);	// assumed finishing RC
						else
							valid_check.write(0);
					break;
				case (1):
					// is allocated for an out port and no new incoming flit
						if (sa_allocated_reg.read() >= 0 && !valid_in_reg.read()){
							valid_check.write(0);
						}
						else{	// not allocated yet or having new incoming flit
							valid_check.write(1);
						}
					break;
			}
	}

	SC_CTOR (ValidCheckUpdate){
		SC_METHOD (valid_check_update_process);
		sensitive << valid_in_reg << sa_allocated_reg;
		sensitive << valid_check_reg;
	}
};


#endif /* VALID_CHECK_UPDATE_H_ */
