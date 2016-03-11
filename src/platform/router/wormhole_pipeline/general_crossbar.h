/*
 * general_crossbar.h
 *
 *  Created on: Nov 28, 2010
 *      Author: Anh Tran
 *
 *  a MxN crossbar with M inputs x N outputs
 */

#ifndef GENERAL_CROSSBAR_H_
#define GENERAL_CROSSBAR_H_

#include <systemc.h>
#include "../../definition.h"
#include "../router_activities.h"

template <int M, int N>
SC_MODULE (GeneralCrossbar){
	sc_in <bool> reset;
	sc_in <bool> clk;

	sc_in <Flit> crossbar_in[M];
	sc_in <bool> crossbar_in_valid[M];

	sc_in <int> crossbar_grant[N];


	sc_out <Flit> crossbar_out[N];
	sc_out <bool> crossbar_out_valid[N];

	// process
	void crossbar_process();

	// activities
	void activity_process();

	CrossbarActivities *getCrossbarActivities();

	SC_CTOR (GeneralCrossbar){
		SC_METHOD (crossbar_process);
		for (int m=0; m<M; m++){
			sensitive << crossbar_in[m];
			sensitive << crossbar_in_valid[m];
		}
		for (int n=0; n<N; n++){
			sensitive << crossbar_grant[n];
		}

		// for crossbar activity estimation
		SC_METHOD (activity_process);
		sensitive << reset.pos() << clk.pos();
	}

private:
	int n_1_output_cycles;
	int n_2_outputs_cycles;
	int n_3_outputs_cycles;
	int n_4_outputs_cycles;
	int n_5_outputs_cycles;
	int n_inactive_cycles;

//	bool last_crossbar_out_valid[N];
};

#endif /* GENERAL_CROSSBAR_H_ */

//================ implementation

/*
 * crossbar process
 */
template <int M, int N>
void GeneralCrossbar<M, N>::crossbar_process(){
	for (int n=0; n<N; n++){
		if (crossbar_grant[n].read() >= 0){	// output port [n] was allocated
			int m = crossbar_grant[n].read();

			crossbar_out_valid[n].write(crossbar_in_valid[m].read());
			crossbar_out[n].write(crossbar_in[m].read());
		}
		else {
			crossbar_out_valid[n].write(0);
			crossbar_out[n].write(Flit());
		}
	}
}

/*
 * getCrossbarActivities
 */
template <int M, int N>
CrossbarActivities *GeneralCrossbar<M, N>::getCrossbarActivities(){
	CrossbarActivities *crossbar_activities = new CrossbarActivities();
	crossbar_activities->n_1_output_cycles = n_1_output_cycles;
	crossbar_activities->n_2_outputs_cycles = n_2_outputs_cycles;
	crossbar_activities->n_3_outputs_cycles = n_3_outputs_cycles;
	crossbar_activities->n_4_outputs_cycles = n_4_outputs_cycles;
	crossbar_activities->n_5_outputs_cycles = n_5_outputs_cycles;
	crossbar_activities->n_inactive_cycles = n_inactive_cycles;

	return crossbar_activities;
}

/*
 * crossbar activity
 */
template <int M, int N>
void GeneralCrossbar<M, N>::activity_process(){
	if (reset.read()){
//		for (int n=0; n<N; n++){
//			last_crossbar_out_valid[n] = 0;
//		}

		n_1_output_cycles = 0;
		n_2_outputs_cycles = 0;
		n_3_outputs_cycles = 0;
		n_4_outputs_cycles = 0;
		n_5_outputs_cycles = 0;
		n_inactive_cycles = 0;
	}
	else {
//		bool current_crossbar_out_valid[N];
//		for (int n=0; n<N; n++){
//			current_crossbar_out_valid[n] = crossbar_out_valid[n].read();
//		}

		// update activities
		int current_time = (int) (sc_time_stamp().to_double()/1000);
		if (current_time >= CommonParameter::warmup_time) {
			int out_count = 0;
			for (int n=0; n<N; n++){
				if (crossbar_out_valid[n].read()) out_count += 1;
			}

			switch (out_count){
			case (0):
					n_inactive_cycles += 1;
					break;
			case (1):
					n_1_output_cycles += 1;
					break;
			case (2):
					n_2_outputs_cycles += 1;
					break;
			case (3):
					n_3_outputs_cycles += 1;
					break;
			case (4):
					n_4_outputs_cycles += 1;
					break;
			case (5):
					n_5_outputs_cycles += 1;
					break;
			default:
					cout << "Crossbar Activity Fails ===> STOP!" << endl;
					exit(0);
			}
		}
	}
}
