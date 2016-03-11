/*
 * evaluation.h
 *
 * (C) copyright by the VCL laboratory, ECE Department, UC Davis
 *
 *  Please send en email to anhtrandavis@gmail.com to ask for your permission
 *  before using this code;
 *  and cite the paper "..." in your related publications.
 *
 *  Created on: Apr 29, 2010
 *      Author: Anh Tran
 *
 *  evaluation of simulation results: latency, throughput, energy, power...
 */

#ifndef PROC_EVALUATION_H_
#define PROC_EVALUATION_H_

struct ProcEvaluationFactors{	// empty, other evaluation inherits this struct

};

struct SyntheticFactors: ProcEvaluationFactors{
	int n_injected_packets;
	int n_received_packets;
	double total_latency;
	double max_latency;
	double min_latency;
};

struct EmbeddedFactors: ProcEvaluationFactors{
	int n_injected_packets;
	int n_received_packets;
	double total_latency;
	double max_latency;
	double min_latency;
};

struct AsAPFactors: ProcEvaluationFactors{
	int first_input_time;		// the cycle time at which the first input is received
	int first_output_time;		// the cycle time at which the first output is sent

	int last_input_time;		// the cycle time at which the last input is received
	int last_output_time;		// the cycle time at which the last output is sent

	int n_input_words;
	int n_output_words;		// number of output words

	int n_input_stall_cycles;
	int n_output_stall_cycles;
};


#endif /* PROC_EVALUATION_H_ */
