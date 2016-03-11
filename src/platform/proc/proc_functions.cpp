/*
 * proc_functions.cpp
 *
 *  Created on: Aug 18, 2011
 *      Author: anhttran
 */

#include "proc_functions.h"

// random interval of packet injection times with exponential distribution
int inter_injection_time(double flit_rate, int type){
	// packet inject rate
	int result = 0;
//	double packet_inj_rate = (double)flit_rate/SyntheticParameter::packet_length;

	double mean_inter_arrival;

	if (ProcessorParameters::packet_length_type == PACKET_LENGTH_TYPE_FIXED)
		mean_inter_arrival =  (double)(ProcessorParameters::packet_length/flit_rate);
	else {
		double mean_packet_length = (double)(ProcessorParameters::min_packet_length + ProcessorParameters::max_packet_length)/2;
		mean_inter_arrival = (double)(mean_packet_length/flit_rate);
	}

//	cout << "mean_inter_arrival = " << mean_inter_arrival << endl;

	switch (type){
		case (INTER_ARRIVAL_EXPONENTIAL):
				double u;
				u = (double)rand()/RAND_MAX;	// a uniform random in [0,1)

				result = (int)(-mean_inter_arrival*log(u));
			break;
		case (INTER_ARRIVAL_IDENTICAL):
				result = (int)(mean_inter_arrival);
			break;
		default:;
	}

	return result;
}

/*
 * uniform random
 */
Flit *create_head_flit_random(int _src_x, int _src_y, int _time){
	int dst_x;
	int dst_y;
	do {
		dst_x = rand()%CommonParameter::dim_x;
		dst_y = rand()%CommonParameter::dim_y;
	} while ((dst_x == _src_x) && (dst_y == _src_y));	// not send packets to itself

	return create_head_flit_fixed_dest(_src_x, _src_y, dst_x, dst_y, _time);
}

/*
 * regional with distance of 3 (80% probability)
 */
Flit *create_head_flit_regional(int _src_x, int _src_y, int _time){

	int dst_x, dst_y;

	int region = rand()%100;

	if (region < SyntheticParameters::regional_percentage){	// regional case
		do {
			dst_x = rand()%CommonParameter::dim_x;
			dst_y = rand()%CommonParameter::dim_y;
		} while ((dst_x == _src_x && dst_y == _src_y) 	// not send packets to itself
				|| (abs(dst_x-_src_x) + abs(dst_y-_src_y) > SyntheticParameters::regional_distance));
	}
	else {	// random case
		do {
			dst_x = rand()%CommonParameter::dim_x;
			dst_y = rand()%CommonParameter::dim_y;
		} while (abs(dst_x-_src_x) + abs(dst_y-_src_y) <= SyntheticParameters::regional_distance);
	}

	return create_head_flit_fixed_dest(_src_x, _src_y, dst_x, dst_y, _time);
}

/*
 * neighboring
 */
Flit *create_head_flit_neighbor(int _src_x, int _src_y, int _time){
	int dst_x, dst_y;

	int region = rand()%100;

	if (region < SyntheticParameters::neighbor_percentage){	// neighboring case
		do {
			dst_x = rand()%CommonParameter::dim_x;
			dst_y = rand()%CommonParameter::dim_y;
		} while ((dst_x == _src_x && dst_y == _src_y) 	// not send packets to itself
				|| (abs(dst_x-_src_x) + abs(dst_y-_src_y) > 1));
	}
	else {	// random case
		do {
			dst_x = rand()%CommonParameter::dim_x;
			dst_y = rand()%CommonParameter::dim_y;
		} while (abs(dst_x-_src_x) + abs(dst_y-_src_y) <= 1);
	}

	return create_head_flit_fixed_dest(_src_x, _src_y, dst_x, dst_y, _time);
}

/*
 * hotspot
 */
Flit *create_head_flit_hotspot(int _src_x, int _src_y, int _time){
	int dst_x, dst_y;

	int region = rand()%100;

	if (region < SyntheticParameters::hotspot_percentage){	// hotspot case
		if (SyntheticParameters::n_hotspots > 1){
			do {
				// pick a random hotspot
				int hotspot_number = rand()%SyntheticParameters::n_hotspots;

	/*			// get the tile_id at the order_th location in the set
				set <TileLocation>::iterator it = SyntheticParameters::hotspot_set.begin();
				while (order != 0){
					it++;
					order -= 1;
				}
	*/
				TileLocation hotspot_loc = SyntheticParameters::hotspot_table.find(hotspot_number)->second;

				dst_x = hotspot_loc.x;
				dst_y = hotspot_loc.y;
			} while (dst_x == _src_x && dst_y == _src_y);
		}
		else {	// n_hotspots == 1
			TileLocation hotspot_loc = SyntheticParameters::hotspot_table.find(0)->second;

			if (_src_x == hotspot_loc.x && _src_y == hotspot_loc.y){	// if source is at the same location with the hotspot
				// choose a random destination
				do {
					dst_x = rand()%CommonParameter::dim_x;
					dst_y = rand()%CommonParameter::dim_y;
				} while (dst_x == _src_x && dst_y == _src_y);
			}
			else {
				// destination is this hotspot
				dst_x = hotspot_loc.x;
				dst_y = hotspot_loc.y;
			}
		}
	}
	else {	// random case
		do {
			dst_x = rand()%CommonParameter::dim_x;
			dst_y = rand()%CommonParameter::dim_y;
		} while (dst_x == _src_x && dst_y == _src_y);
	}

	return create_head_flit_fixed_dest(_src_x, _src_y, dst_x, dst_y, _time);
}

/*
 * transpose
 */
Flit *create_head_flit_transpose(int _src_x, int _src_y, int _time){
	int dst_x = _src_y;
	int dst_y = _src_x;

	return create_head_flit_fixed_dest(_src_x, _src_y, dst_x, dst_y, _time);
}

/*
 * bit-complement
 */
Flit *create_head_flit_bit_complement(int _src_x, int _src_y, int _time){
	// number of bits presenting the network dimension
	int nbits = (int) ceil(log((double)CommonParameter::dim_x) / log(2.0));

	int dst_x = bit_complement(_src_x, nbits);
	int dst_y = bit_complement(_src_y, nbits);

	return create_head_flit_fixed_dest(_src_x, _src_y, dst_x, dst_y, _time);
}

/*
 * bit-reversal
 * x3x2x1x0 --> x0x1x2x3
 */
Flit *create_head_flit_bit_reversal(int _src_x, int _src_y, int _time){
	// number of bits presenting the network dimension
	int nbits = (int) ceil(log((double)CommonParameter::dim_x) / log(2.0));

	TileLocation src_tile = TileLocation(_src_x, _src_y);

	TileLocation dst_tile = complete_bit_reversal(src_tile, nbits);

	int dst_x = dst_tile.x;
	int dst_y = dst_tile.y;

	return create_head_flit_fixed_dest(_src_x, _src_y, dst_x, dst_y, _time);
}

/*
 * complete shuffle
 */
Flit *create_head_flit_shuffle(int _src_x, int _src_y, int _time){
	// number of bits presenting the network dimension
	int nbits = (int) ceil(log((double)CommonParameter::dim_x) / log(2.0));

	TileLocation src_tile = TileLocation(_src_x, _src_y);

	TileLocation dst_tile = complete_shuffle(src_tile, nbits);

	int dst_x = dst_tile.x;
	int dst_y = dst_tile.y;

	return create_head_flit_fixed_dest(_src_x, _src_y, dst_x, dst_y, _time);
}

/*
 * rotation to right 1 bit
 */
Flit *create_head_flit_rotate(int _src_x, int _src_y, int _time){
	// number of bits presenting the network dimension
	int nbits = (int) ceil(log((double)CommonParameter::dim_x) / log(2.0));

	TileLocation src_tile = TileLocation(_src_x, _src_y);

	TileLocation dst_tile = complete_right_rotate(src_tile, nbits, 1);

	int dst_x = dst_tile.x;
	int dst_y = dst_tile.y;

	return create_head_flit_fixed_dest(_src_x, _src_y, dst_x, dst_y, _time);
}


/*
 * tornado
 */
Flit *create_head_flit_tornado(int _src_x, int _src_y, int _time){
	int dst_x = (_src_x + (int)(CommonParameter::dim_x/2) - 1) % CommonParameter::dim_x;
	int dst_y = (_src_y + (int)(CommonParameter::dim_x/2) - 1) % CommonParameter::dim_y;

	return create_head_flit_fixed_dest(_src_x, _src_y, dst_x, dst_y, _time);
}

/*
 * ACK flit
 */
Flit *create_head_flit_ACK(int _src_x, int _src_y, int _dst_x, int _dst_y, int _time){
	Flit *flit_tmp = new Flit();

	flit_tmp->head = 1;
	flit_tmp->tail = 1;
	flit_tmp->src_x = _src_x;
	flit_tmp->src_y = _src_y;

	flit_tmp->dst_x = _dst_x;
	flit_tmp->dst_y = _dst_y;

	flit_tmp->payload = 43690;	// = AAAA in Hex, only using for notice of a ACK flit in waveform
	flit_tmp->injected_time = _time;
	flit_tmp->fifo_id = 0;
	flit_tmp->vc_id = 0;

	flit_tmp->type = 1;	// ACK

	flit_tmp->ACK_required = 0;

	return flit_tmp;
}

/*
 * fixed destination
 */
Flit *create_head_flit_fixed_dest(int _src_x, int _src_y, int _dst_x, int _dst_y, int _time){
	Flit *flit_tmp = new Flit();

	flit_tmp->head = 1;
	flit_tmp->tail = 0;
	flit_tmp->src_x = _src_x;
	flit_tmp->src_y = _src_y;

	flit_tmp->dst_x = _dst_x;
	flit_tmp->dst_y = _dst_y;

	flit_tmp->payload = rand();
	flit_tmp->injected_time = _time;
	flit_tmp->fifo_id = 0;
	flit_tmp->vc_id = 0;

	flit_tmp->type = 0;	// data

	return flit_tmp;

}
