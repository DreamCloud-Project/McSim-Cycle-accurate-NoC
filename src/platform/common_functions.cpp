/*
 * common_functions.cpp
 *
 *  Created on: May 3, 2010
 *      Author: Anh Tran
 *
 *  Implementation of common_functions.h
 */

#include "common_functions.h"


//
// write a text to an output file
//
void write_to_file(ofstream file_out, char *text){
	file_out << text << endl;
}

// convert an integer number to string
string int_to_str(int x) {
	ostringstream o;
	if (!(o << x))
//		throw BadConversion("int_to_str(int)");
		throw runtime_error("int_to_str(int");
	return o.str();
}

// convert an ordinary number to direction
string int2direction(int i){
	string direction;
	switch (i){
		case (WEST):
				direction = "W";
			break;
		case (EAST):
				direction = "E";
			break;
		case (NORTH):
				direction = "N";
			break;
		case (SOUTH):
				direction = "S";
			break;
		case (LOCAL):
				direction = "L";
			break;
		default:
			cout << "ERROR: direction is invalid!" << endl;
			exit (0);
			break;
	}

	return direction;
}

// convert a tile coordinate location (x,y) to tile_id
int tile_loc_to_id(TileLocation tile_loc, int dim_x, int dim_y){
	return (tile_loc.x + tile_loc.y*dim_x);
}

// convert a tile_id to tile coordinate (x,y)
TileLocation tile_id_to_loc(int id, int dim_x, int dim_y){
	TileLocation tile_loc;
	tile_loc.x = id % dim_x;
	tile_loc.y = (int) (id/dim_x);

	return (tile_loc);
}

// get bit value at position p of x
int getBit(int x, int p){
	int tmp = x >> p;
	return (tmp & 1);
}

// set bit at position p of x to value v
int setBit(int x, int p, int v){
	if ((v != 1) && (v!= 0))
		cout << "ERROR: the setBit value is invalid!";
	assert ((v==1) || (v==0));

	int mask = 1 << p;
	if (v == 1)
		return (x | mask);
	else	// v == 0
		return (x & ~mask);
}

/*
* find bit-complement of x with n bits
*/
int bit_complement(int x, int n){
	int tmp = 0;
	for (int i=0; i<=n-1; i++){
		int u = getBit(x, i);

		// bit complement
		int v = (u==0)?1:0;

		tmp = setBit(tmp, i, v);
	}

	return tmp;
}

/*
 * bit-reversal of x with n bits
 */
int bit_reversal(int x, int n){
	int tmp = 0;
	for (int i=0; i<n; i++){
		int u = getBit(x,i);

		tmp = setBit(tmp, n-1-i, u);
	}

	return tmp;
}

/*
 * shuffle = turn left by 1 bit
 */
int shuffle(int x, int n){
	return left_rotate(x, n, 1);
}

/*
 * left rotation
 */
int left_rotate(int x, int n, int m){
	int tmp = x << m; // shift left x by m bits

	for (int i=0; i<m; i++){
		int u = getBit(x, n-1-i);

		tmp = setBit(tmp, m-1-i, u);
	}

	return tmp;
}

/*
 * right rotation
 */
int right_rotate(int x, int n, int m){
	int tmp = x >> m;

	for (int i=0; i<m; i++){
		int u = getBit(x, i);

		tmp = setBit(tmp,n-m+i, u);
	}

	return tmp;
}

/*
 * butterfly: exchange the last bit with the first bit
 */
int butterfly(int x, int n){
	return 0;	// will do it later if needed
}

/*
 * complete bit-reversal of the whole address(Y,X)
 */
TileLocation complete_bit_reversal(TileLocation tile_loc, int n){
	int x = tile_loc.x;
	int y = tile_loc.y;


	int addr = x << n;
	addr = addr + y;

	int addr_bit_reversal = bit_reversal(addr, 2*n);	// 2n is total address bits, bot y and x directions

	int mask = (1<<n) - 1;	// n bit '111...1'

	int x_tmp = (addr_bit_reversal >> n) & mask;	// take n MSB bits
	int y_tmp = addr_bit_reversal & mask;	// take n LSB bits

	TileLocation tile_tmp = TileLocation(x_tmp, y_tmp);

	return tile_tmp;
}

/*
 * complete shuffle
 */
TileLocation complete_shuffle(TileLocation tile_loc, int n){
	int x = tile_loc.x;
	int y = tile_loc.y;


	int addr = x << n;
	addr = addr + y;

	int addr_bit_reversal = shuffle(addr, 2*n);	// 2n is total address bits, bot y and x directions

	int mask = (1<<n) - 1;	// n bit '111...1'

	int x_tmp = (addr_bit_reversal >> n) & mask;	// take n MSB bits
	int y_tmp = addr_bit_reversal & mask;	// take n LSB bits

	TileLocation tile_tmp = TileLocation(x_tmp, y_tmp);

	return tile_tmp;
}

/*
 * complete left rotation
 */
TileLocation complete_left_rotate(TileLocation tile_loc, int n, int m){
	int x = tile_loc.x;
	int y = tile_loc.y;

	int addr = x << n;
	addr = addr + y;

	int addr_bit_reversal = left_rotate(addr, 2*n, m);	// 2n is total address bits, bot y and x directions

	int mask = (1<<n) - 1;	// n bit '111...1'

	int x_tmp = (addr_bit_reversal >> n) & mask;	// take n MSB bits
	int y_tmp = addr_bit_reversal & mask;	// take n LSB bits

	TileLocation tile_tmp = TileLocation(x_tmp, y_tmp);

	return tile_tmp;
}

/*
 * complete right rotation
 */
TileLocation complete_right_rotate(TileLocation tile_loc, int n, int m){
	int x = tile_loc.x;
	int y = tile_loc.y;

	int addr = x << n;
	addr = addr + y;

	int addr_bit_reversal = right_rotate(addr, 2*n, m);	// 2n is total address bits, bot y and x directions

	int mask = (1<<n) - 1;	// n bit '111...1'

	int x_tmp = (addr_bit_reversal >> n) & mask;	// take n MSB bits
	int y_tmp = addr_bit_reversal & mask;	// take n LSB bits

	TileLocation tile_tmp = TileLocation(x_tmp, y_tmp);

	return tile_tmp;
}
