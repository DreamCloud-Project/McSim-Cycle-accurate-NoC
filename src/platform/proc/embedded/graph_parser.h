/*
 * graph_parser.h
 *
 *  Created on: Aug 15, 2011
 *      Author: anhttran
 */

#ifndef GRAPH_PARSER_H_
#define GRAPH_PARSER_H_

#include <iostream>
#include <fstream>
#include <string.h>
#include "../../common_parameters.h"


using namespace std;
//using namespace boost;

class AppGraph{	// application graph
public:
	int n_tasks;	// <= MAX_DIM
	double com_bandwidth[MAX_N_TASKS][MAX_N_TASKS];	// communication bandwidth requirement

	// constructor
/*	AppGraph(){
		n_tasks = MAX_DIM*MAX_DIM;
		for (int i=0; i<MAX_DIM; i++){
			for (int j=0; j<MAX_DIM; j++){
				com_bandwidth[i][j] = 0;
			}
		}
	}
*/
	AppGraph(const char *filename);

private:
	void app_graph_parser(ifstream app_file);
};

// return graph info of an application from an input text file
//AppGraph app_graph_parser(const char *filename);

#endif /* GRAPH_PARSER_H_ */
