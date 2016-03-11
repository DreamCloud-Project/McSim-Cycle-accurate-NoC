/*
 * task_mapping.h
 *
 *  Created on: Aug 15, 2011
 *      Author: Anh Tran
 *
 *  Implementation of algorithms mapping tasks to a 2-D mesh network of tiles
 */

#ifndef TASK_MAPPING_H_
#define TASK_MAPPING_H_

#include "../../common_functions.h"
#include "graph_parser.h"
#include <math.h>
#include <set>
#include <float.h>
#include "embedded_parameters.h"

class TaskMapping{
public:
	int dim_x;
	int dim_y;

	int task_mapping[MAX_DIM*MAX_DIM]; 	// mapping task(i) --> tile_id

	// constructor
	TaskMapping(AppGraph app_graph, int mapping_algo);

private:
	// network dimension from n_tasks
	void network_dim_cal(int n_tasks, int &dim_x, int &dim_y);

	// mapping using NMAP algorithm
	void nmap_algorithm(AppGraph app_graph);

	// random mapping
	void random_algorithm(AppGraph app_graph);

};


#endif /* TASK_MAPPING_H_ */
