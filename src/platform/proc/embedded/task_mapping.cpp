/*
 * task_mapping.cpp
 *
 *  Created on: Aug 15, 2011
 *      Author: anhttran
 */

#include "task_mapping.h"

/*
 * constructor
 */
TaskMapping::TaskMapping(AppGraph app_graph, int mapping_algo){
	switch (mapping_algo){
	case MAPPING_RANDOM:
		cout << "Mapping tasks to the network using RANDOM algorithm......." <<endl;
		random_algorithm(app_graph);
		break;
	case MAPPING_NMAP:
		cout << "Mapping tasks to the network using NMAP algorithm........" <<endl;
		nmap_algorithm(app_graph);
		break;
	default:;
	}
}


/*
 * calculate network dimension from number of tasks
 */
void TaskMapping::network_dim_cal(int n_tasks, int &dim_x, int &dim_y){
	dim_y = (int)floor(sqrt(n_tasks));
	dim_x = (int)ceil(n_tasks/dim_y);
}

/*
 * task mapping using NMAP algorithm
 */
void TaskMapping::nmap_algorithm(AppGraph app_graph){

	int n_tasks = app_graph.n_tasks;

	// find near-optimal dim_x and dim_y of the 2-D array
	network_dim_cal(n_tasks, dim_x, dim_y);

//	cout << "dim_x = " << dim_x << "; dim_y = " << dim_y << endl;

	set <int> unmapped_tasks_set; // unmapped set of tasks
	set <int> mapped_tasks_set;
	set <int> unallocated_tiles_set;	// tile ids which are not allocated yet
	set <int> allocated_tiles_set;

	unmapped_tasks_set.clear();
	mapped_tasks_set.clear();
	unallocated_tiles_set.clear();
	allocated_tiles_set.clear();

	for (int i=0; i<n_tasks; i++){
		unmapped_tasks_set.insert(i);
	}

	for (int id=0; id<dim_x*dim_y; id++){
		unallocated_tiles_set.insert(id);
	}

	//------ step 1: find the task with highest weighted communication volume
	// find the max of com_vol
	// consider all incoming and outgoing connections of each tasks
	double max_com = 0;
	int max_com_task;
	for (int i=0; i<app_graph.n_tasks; i++){
		double com_vol = 0;
		for (int j=0; j<app_graph.n_tasks; j++){
			if (app_graph.com_bandwidth[i][j]>0)
				com_vol += app_graph.com_bandwidth[i][j];
			if (app_graph.com_bandwidth[j][i]>0)
				com_vol += app_graph.com_bandwidth[j][i];
		}

		if (com_vol > max_com){
			max_com = com_vol;
			max_com_task = i;
		}
	}

	// add this task with highest weighted communication volume to the mapped task set
	mapped_tasks_set.insert(max_com_task);
	unmapped_tasks_set.erase(max_com_task);

	//------ step 2: find the tile with max number of neighbors
	// normally, this tile is in the middle of the array
	int x_mid = (int)floor(dim_x/2);
	int y_mid = (int)floor(dim_y/2);
	TileLocation tile_loc = TileLocation(x_mid, y_mid);

	int max_neighbors_tile_id = tile_loc_to_id(tile_loc, dim_x, dim_y);


	allocated_tiles_set.insert(max_neighbors_tile_id);
	unallocated_tiles_set.erase(max_neighbors_tile_id);

	// map the task with highest communication volume to the tile with largest number of neighbors
	task_mapping[max_com_task] = max_neighbors_tile_id;

	cout << "Map task [" << max_com_task << "] to Tile[" << tile_loc.x << "," << tile_loc.y << "]" << endl;

	//------ step 3: map all unmapped tasks
	while (!unmapped_tasks_set.empty()){
		//--------- step 3.1:
		// find the unmapped task which communicates most with mapped_tasks_set
		// among many tasks which have the same communication volume with mapped_tasks,
		// choose the task with highest communication volume with others
		double max_com = 0;
		int max_com_unmapped_task;
		double max_overall_com = 0;
		set <int>::iterator unmapped_task_it;
		for (unmapped_task_it=unmapped_tasks_set.begin(); unmapped_task_it!=unmapped_tasks_set.end(); unmapped_task_it++){
			double com_vol = 0;

			int unmapped_task = *unmapped_task_it;

			set <int>::iterator mapped_task_it;
			for (mapped_task_it=mapped_tasks_set.begin(); mapped_task_it!=mapped_tasks_set.end(); mapped_task_it++){
				int mapped_task = *mapped_task_it;

				if (app_graph.com_bandwidth[unmapped_task][mapped_task]>0)
					com_vol += app_graph.com_bandwidth[unmapped_task][mapped_task];
				if (app_graph.com_bandwidth[mapped_task][unmapped_task]>0)
					com_vol += app_graph.com_bandwidth[mapped_task][unmapped_task];
			}

			// this loop using to choose the task with highest comm volume
			// if having more than one task having the same volume with mapped_tasks

			double overall_com_vol = 0;
			for (int scan=0; scan<n_tasks; scan++){
				overall_com_vol += app_graph.com_bandwidth[unmapped_task][scan];
				overall_com_vol += app_graph.com_bandwidth[scan][unmapped_task];
			}

			if (com_vol > max_com){
				max_com = com_vol;
				max_com_unmapped_task = unmapped_task;
				max_overall_com = overall_com_vol;
			}
			else if (com_vol == max_com){ // choose if it have higher comm volume
				if (overall_com_vol > max_overall_com){
					max_com_unmapped_task = unmapped_task;
					max_overall_com = overall_com_vol;
				}
			}
		}


		//--------- step 3.2, find the unallocated tile with lowest communication cost to/from allocated_tile_set
		double min_com_cost = DBL_MAX;
		int min_com_cost_tile_id;
		set <int>::iterator unallocated_tile_it;
		for (unallocated_tile_it=unallocated_tiles_set.begin(); unallocated_tile_it!=unallocated_tiles_set.end(); unallocated_tile_it++){
			double com_cost = 0;

			int unallocated_x = tile_id_to_loc(*unallocated_tile_it, dim_x, dim_y).x;
			int unallocated_y = tile_id_to_loc(*unallocated_tile_it, dim_x, dim_y).y;

			// scan all mapped tasks
			set <int>::iterator mapped_task_it;
			for (mapped_task_it=mapped_tasks_set.begin(); mapped_task_it!=mapped_tasks_set.end(); mapped_task_it++){
				int mapped_task = *mapped_task_it;

				// get location of this mapped task
//				TileLocation allocated_tile_loc = task_map.mapped_task[mapped_task];
				int allocated_tile_loc_id = task_mapping[mapped_task];

				TileLocation allocated_tile_loc = tile_id_to_loc(allocated_tile_loc_id, dim_x, dim_y);

				int allocated_x = allocated_tile_loc.x;
				int allocated_y = allocated_tile_loc.y;

				// mahattan distance of 2 tiles
				int mah_distance = abs(unallocated_x - allocated_x) + abs(unallocated_y - allocated_y);

				if (app_graph.com_bandwidth[max_com_unmapped_task][mapped_task]>0)
					com_cost += app_graph.com_bandwidth[max_com_unmapped_task][mapped_task]*mah_distance;
				if (app_graph.com_bandwidth[mapped_task][max_com_unmapped_task]>0)
					com_cost += app_graph.com_bandwidth[mapped_task][max_com_unmapped_task]*mah_distance;

			}

			if (com_cost < min_com_cost){
				min_com_cost = com_cost;
				min_com_cost_tile_id = *unallocated_tile_it;
			}
		}

		// add max_com_unmapped_task to the mapped_tasks_set set
		unmapped_tasks_set.erase(max_com_unmapped_task);
		mapped_tasks_set.insert(max_com_unmapped_task);

		// add min_com_cost_tile to the allocated_tiles_set
		unallocated_tiles_set.erase(min_com_cost_tile_id);
		allocated_tiles_set.insert(min_com_cost_tile_id);


		TileLocation allocated_tile_loc = tile_id_to_loc(min_com_cost_tile_id, dim_x, dim_y);

		task_mapping[max_com_unmapped_task] = min_com_cost_tile_id;

		cout << "Map task [" << max_com_unmapped_task << "] to Tile[" << allocated_tile_loc.x << "," << allocated_tile_loc.y << "]" << endl;
	}

	cout << "Task Mapping Done---!" << endl;
}


/*
 * random mapping
 */
void TaskMapping::random_algorithm(AppGraph app_graph){

	int n_tasks = app_graph.n_tasks;

	// find near-optimal dim_x and dim_y of the 2-D array
	network_dim_cal(n_tasks, dim_x, dim_y);

	set <int> unallocated_tiles_set;	// tile ids which are not allocated yet

	unallocated_tiles_set.clear();

	for (int id=0; id<dim_x*dim_y; id++){
		unallocated_tiles_set.insert(id);
	}

	// randomly pick tile location for each task
	for (int i=0; i<n_tasks; i++){	// task_i
		// number of unallocated tiles
		int remain = unallocated_tiles_set.size();

		// pick a random number
		int order = rand()%remain;

//		cout << "remain = " << remain << "; order = " << order << endl;

		// get the tile_id at the order_th location in the set
		set <int>::iterator it = unallocated_tiles_set.begin();
		while (order != 0){
			it++;
			order -= 1;
		}

		// remove the tile from unallocated set
		unallocated_tiles_set.erase(*it);

		// map the tile to the task
		TileLocation tile_loc = tile_id_to_loc(*it, dim_x, dim_y);
		task_mapping[i] =  *it;

		cout << "Map task [" << i << "] to Tile[" << tile_loc.x << "," << tile_loc.y << "]" << endl;
	}

	cout << "Task Mapping Done---!" << endl;
}
