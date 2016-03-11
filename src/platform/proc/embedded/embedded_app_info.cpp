/*
 * embedded_app_info.cpp
 *
 *  Created on: Aug 17, 2011
 *      Author: anhttran
 */

#include "embedded_app_info.h"

EmbeddedAppInfo::EmbeddedAppInfo(const char *filename){
	cout << "============================================" << endl;

	// parse application graph from the input file
	AppGraph *app_graph = new AppGraph(filename);
	int n_tasks = app_graph->n_tasks;

	// mapping tasks to a 2-D mesh of tiles
	cout << "============================================" << endl;

	TaskMapping *task_map = new TaskMapping(*app_graph, EmbeddedParameters::mapping_algorithm);

	dim_x = task_map->dim_x;
	dim_y = task_map->dim_y;

	//--------- get overall application info
	// initialize
	unused_tiles_set.clear();
	for (int id=0; id<dim_x*dim_y; id++)
		unused_tiles_set.insert(id);

	app_info.clear();

	max_req_bandwidth = 0;

	// scan the graph and mapping info
	for (int task_id=0; task_id<n_tasks; task_id++){
		int tile_id = task_map->task_mapping[task_id];

		TileInfo tile_info;

		unused_tiles_set.erase(tile_id);	// remove this tile from the unused set

		int dsts_count = 0;	// number of destinations of this tile
		for (int dst_task=0; dst_task<n_tasks; dst_task++){
			if (app_graph->com_bandwidth[task_id][dst_task]>0){
				tile_info.dst_info[dsts_count].dst_tile_id = task_map->task_mapping[dst_task];
				tile_info.dst_info[dsts_count].bandwidth = app_graph->com_bandwidth[task_id][dst_task];

				dsts_count += 1;

				if (app_graph->com_bandwidth[task_id][dst_task] > max_req_bandwidth)
					max_req_bandwidth = app_graph->com_bandwidth[task_id][dst_task];
			}
		}

		tile_info.n_dsts = dsts_count;

		app_info.insert(EmbeddedAppHashTable::value_type(tile_id, tile_info));
	}

	cout << "max_req_bandwidth = " << max_req_bandwidth << endl;

	delete task_map;
	delete app_graph;
}
