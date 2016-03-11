/*
 * embedded_evaluation.cpp
 *
 *  Created on: Aug 21, 2011
 *      Author: anhttran
 */

#include "embedded_evaluation.h"

EmbeddedEvaluation::EmbeddedEvaluation(Platform *_platform){
	platform = _platform;

	// initializing calculation
	initialize();
}

void EmbeddedEvaluation::initialize(){
	EmbeddedFactors *embedded_factors = NULL;

	total_latency = 0;
	total_rx_packets = 0;
	for (int x=0; x<=CommonParameter::dim_x-1; x++){
		for (int y=0; y<=CommonParameter::dim_y-1; y++){
			embedded_factors = (EmbeddedFactors*) platform->tile[x][y]->proc->evaluation();
			total_latency += embedded_factors->total_latency;
			total_rx_packets += embedded_factors->n_received_packets;

			cout << "total latency at [" << x << "," << y << "]= "
					<< embedded_factors->total_latency << "      ;";
			cout << "total packets received at [" << x << "," << y << "]= "
					<< embedded_factors->n_received_packets << endl;
		}
	}

	cout << "total_latency :" << total_latency << endl;
	cout << "total_rx_packets :" << total_rx_packets << endl;

	if (CommonParameter::sim_mode == SIM_MODE_CYCLE){
		GlobalVariables::n_total_rx_packets = total_rx_packets;
	}

	delete (embedded_factors);
}

double EmbeddedEvaluation::avg_latency_cal(){
	return (double)(total_latency) / (double)total_rx_packets;
}

double EmbeddedEvaluation::avg_throughput_cal(){
	double tmp;
	if (CommonParameter::sim_mode == SIM_MODE_CYCLE){
//		tmp =  (double)(total_rx_packets)
//					/ (CommonParameter::dim_x * CommonParameter::dim_y)
//					/ (CommonParameter::simulation_time - CommonParameter::warmup_time);

		GlobalVariables::last_simulation_time = CommonParameter::simulation_time;
	}
//	else {
		tmp =  (double)(total_rx_packets)
					/ (CommonParameter::dim_x * CommonParameter::dim_y)
					/ (GlobalVariables::last_simulation_time - CommonParameter::warmup_time);
//	}

	return tmp;
}
