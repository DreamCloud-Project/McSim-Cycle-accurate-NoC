/*
 * common_functions.h
 *
 *  Created on: May 3, 2010
 *      Author: Anh Tran
 *
 *  Some useful common functions
 */

#ifndef COMMON_FUNCTIONS_H_
#define COMMON_FUNCTIONS_H_

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cassert>
#include "common_parameters.h"

using namespace std;

/*
class BadConversion: public runtime_error {
  public:
	BadConversion(string const& s):runtime_error(s){ }
 };
*/

void write_to_file(ofstream file_out, char *text);

struct TileLocation{
	int x;
	int y;
	TileLocation(){
		x = 0;
		y = 0;
	}

	TileLocation(int _x, int _y){
		x = _x;
		y = _y;
	}
};

// change from integer to string
string int_to_str(int x);

string int2direction(int i);

// convert from tile_location (x,y) to tile_id
int tile_loc_to_id(TileLocation tile_loc, int dim_x, int dim_y);

// convert a tile_id to tile_location (x,y)
TileLocation tile_id_to_loc(int id, int dim_x, int dim_y);

// get a bit of x at the position p
int getBit(int x, int p);
// set a bit of x at the position p by the value v
int setBit(int x, int p, int v);
// compute the bit-complement of a n-bit number
int bit_complement(int x, int n);

// bit-reversal: x3x2x1x0 --> x0x1x2x3
int bit_reversal(int x, int n);

// bit-shuffle: left rotate a n-bit number by 1 bit
int shuffe(int x, int n);

// left rotate: rotate to lelf m bit
int left_rotate(int x, int n, int m);

// right rotate: rotate to right m bit
int right_rotate(int x, int n, int m);

// butterfly
int butterfly(int x, int n);

// whole address bit_revesal
TileLocation complete_bit_reversal(TileLocation tile_loc, int n);

TileLocation complete_shuffle(TileLocation tile_loc, int n);

TileLocation complete_left_rotate(TileLocation tile_loc, int n, int m);

TileLocation complete_right_rotate(TileLocation tile_loc, int n, int m);

#endif /* COMMON_FUNCTIONS_H_ */

