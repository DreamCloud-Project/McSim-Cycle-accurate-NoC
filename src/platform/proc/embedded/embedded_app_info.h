/*
 * embedded_app_info.h
 *
 *  Created on: Aug 17, 2011
 *      Author: anhttran
 */

#ifndef EMBEDDED_APP_INFO_H_
#define EMBEDDED_APP_INFO_H_

#include "embedded_parameters.h"
#include "graph_parser.h"
#include "task_mapping.h"

using namespace std;


class EmbeddedAppInfo{
public:
	int dim_x;
	int dim_y;

	set <int> unused_tiles_set;

	double max_req_bandwidth;		// max required bandwidth on all links

	EmbeddedAppHashTable app_info;

	// constructor
	EmbeddedAppInfo(const char *filename);
};

#endif /* EMBEDDED_APP_INFO_H_ */
