/*
 * out_credit_remain_update.h
 *
 *  Created on: Dec 31, 2011
 *      Author: anhttran
 */

#ifndef OUT_CREDIT_REMAIN_UPDATE_H_
#define OUT_CREDIT_REMAIN_UPDATE_H_

#include "systemc.h"
#include "../../router_parameters.h"
#include "../../router_activities.h"

SC_MODULE (OutCreditRemainUpdate){
	int out_port;

	sc_in <bool> reset;
	sc_in <bool> clk;

	sc_in <bool> credit_plus;
	sc_in <bool> credit_minus;
	sc_in <int> out_credit_remain_reg;

	sc_out <int> out_credit_remain;

	void out_credit_remain_process(){
		int tmp = out_credit_remain_reg.read();
		if (credit_plus.read())
			tmp += 1;
		if (credit_minus.read())
			tmp -= 1;

		out_credit_remain.write(tmp);
	}

	OutCreditCounterActivities *get_out_credit_counter_activities(){
		OutCreditCounterActivities *out_credit_counter_activities = new OutCreditCounterActivities();
		out_credit_counter_activities->n_one_on_cycles = n_one_on_cycles;
		out_credit_counter_activities->n_one_changes_cycles = n_one_changes_cycles;
		out_credit_counter_activities->n_both_change_diff_cycles = n_both_change_diff_cycles;
		out_credit_counter_activities->n_inactive_cycles = n_inactive_cycles;

		return out_credit_counter_activities;
	}

	void activity_proccess(){
		if (reset.read()){
			last_plus = 0;
			last_minus = 0;

			n_one_on_cycles = 0;
			n_one_changes_cycles = 0;
			n_both_change_diff_cycles = 0;
			n_inactive_cycles = 0;
		}
		else {	// clk.pos
			bool current_plus = credit_plus.read();
			bool current_minus = credit_minus.read();

			// update activities
			int current_time = (int) (sc_time_stamp().to_double()/1000);
			if (current_time >= CommonParameter::warmup_time) {
				if (current_plus == current_minus){
					if (last_plus == last_minus)
						n_inactive_cycles += 1;
					else
						n_one_changes_cycles += 1;
				}
				else {
					if (last_plus != last_minus){
						if (current_plus != last_plus)
							n_both_change_diff_cycles += 1;
						else
							n_one_on_cycles += 1;
					}
					else
						n_one_changes_cycles += 1;
				}
			}

			last_plus = current_plus;
			last_minus = current_minus;
		}
	}

	SC_CTOR (OutCreditRemainUpdate){
		SC_METHOD (out_credit_remain_process);
		sensitive << out_credit_remain_reg << credit_plus << credit_minus;

		// activity process;
		SC_METHOD (activity_proccess);
		sensitive << reset.pos() << clk.pos();
	}


private:
	int n_one_on_cycles;
	int n_one_changes_cycles;
	int n_both_change_diff_cycles;
	int n_inactive_cycles; // both off or both on

	bool last_plus;
	bool last_minus;
};


#endif /* OUT_CREDIT_REMAIN_UPDATE_H_ */
