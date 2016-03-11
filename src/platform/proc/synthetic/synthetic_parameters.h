/*
 * synthetic_parameters.h
 *
 *  Created on: May 24, 2010
 *      Author: Anh Tran
 *
 * Parameters
 */

#include <set>
#include <map>
#include "../../common_functions.h"

#ifndef SYNTHETIC_PARAMETERS_H_
#define SYNTHETIC_PARAMETERS_H_


#define TRAFFIC_RANDOM 1
#define TRAFFIC_TRANSPOSE 2
#define TRAFFIC_BIT_COMPLEMENT 3
#define TRAFFIC_BIT_REVERSAL 4
#define TRAFFIC_TORNADO 5
#define TRAFFIC_NEIGHBOR 6	// 80% packets are sent to 4 neighbors, 20% random
#define TRAFFIC_REGIONAL 7	// 80% of packets' destinations are in 3-hop away from its' sources
#define TRAFFIC_HOTSPOT 8
#define TRAFFIC_SHUFFLE 9
#define TRAFFIC_ROTATE 10


#define NEIGHBOR_PERCENTAGE 80

#define REGIONAL_PERCENTAGE 80
#define REGIONAL_DISTANCE 3

#define HOTSPOT_CENTRAL 1
#define HOTSPOT_CORNER 2
#define HOTSPOT_PERCENTAGE 30

typedef map<int, TileLocation>  HotSpotHashTable;
//typedef set <TileLocation> HotSpotSet;

struct SyntheticParameters{
	static int traffic_type;

	static int neighbor_percentage;
	static int regional_percentage;
	static int regional_distance;

	static int hotspot_type;	// at central or corners
	static int n_hotspots;	// number of hotspots
	static HotSpotHashTable hotspot_table;

	static int hotspot_percentage;


};

#endif /* SYNTHETIC_PARAMETERS_H_ */
