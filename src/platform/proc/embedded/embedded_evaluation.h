/*
 * embedded_evaluation.h
 *
 *  Created on: Aug 21, 2011
 *      Author: anhttran
 */

#ifndef EMBEDDED_EVALUATION_H_
#define EMBEDDED_EVALUATION_H_

#include "../../platform.h"
#include "../../global_variables.h"

class EmbeddedEvaluation{
public:
	// constructor
	EmbeddedEvaluation (Platform *_platform);

	double avg_latency_cal();
	double avg_throughput_cal();

private:
	Platform *platform;

	double total_latency;
	int total_rx_packets;

	// compute total_latency, total_rx_packets,
	void initialize();
};

#endif /* EMBEDDED_EVALUATION_H_ */
