/*
 * proc_functions.h
 *
 *  Created on: Aug 18, 2011
 *      Author: anhttran
 */

#ifndef PROC_FUNCTIONS_H_
#define PROC_FUNCTIONS_H_

#include "../definition.h"
#include "../common_functions.h"
#include "proc_parameters.h"
#include "synthetic/synthetic_parameters.h"

using namespace std;

// random interval of packet injection times with exponential distribution
int inter_injection_time(double flit_rate, int type);

Flit *create_head_flit_random(int _src_x, int _src_y, int _time);

Flit *create_head_flit_transpose(int _src_x, int _src_y, int _time);

Flit *create_head_flit_bit_complement(int _src_x, int _src_y, int _time);

Flit *create_head_flit_bit_reversal(int _src_x, int _src_y, int _time);

Flit *create_head_flit_tornado(int _src_x, int _src_y, int _time);

Flit *create_head_flit_regional(int _src_x, int _src_y, int _time);

Flit *create_head_flit_neighbor(int _src_x, int _src_y, int _time);

Flit *create_head_flit_hotspot(int _src_x, int _src_y, int _time);

Flit *create_head_flit_shuffle(int _src_x, int _src_y, int _time);

Flit *create_head_flit_rotate(int _src_x, int _src_y, int _time);

Flit *create_head_flit_ACK(int _src_x, int _src_y, int _dst_x, int _dst_y, int _time);

Flit *create_head_flit_fixed_dest(int _src_x, int _src_y, int _dst_x, int _dst_y, int _time);



#endif /* PROC_FUNCTIONS_H_ */
