/*
 * timing_parameters.h
 *
 *  Created on: Mar 26, 2012
 *      Author: anhttran
 */

#ifndef TIMING_PARAMETERS_H_
#define TIMING_PARAMETERS_H_

#define STANDARD_65nm_DELAY_SETUP_1_3V			18	// ps
#define STANDARD_65nm_DELAY_CLK_Q_1_3V			71

#define STANDARD_65nm_DELAY_BW_2FLITS_1_3V		137 // 198
#define STANDARD_65nm_DELAY_BW_4FLITS_1_3V		274 //329
#define STANDARD_65nm_DELAY_BW_8FLITS_1_3V		411 // 461
#define STANDARD_65nm_DELAY_BW_16FLITS_1_3V		548 // 593

#define STANDARD_65nm_DELAY_RC_XY_1_3V			459 // RC:418; LRC: 459
#define STANDARD_65nm_DELAY_RC_ADAPTIVE_1_3V	528	// RC: 502; LRC: 528

#define STANDARD_65nm_DELAY_SA_1_3V				302
#define STANDARD_65nm_DELAY_ST_1_3V				325 //350
#define STANDARD_65nm_DELAY_LT_1000um_1_3V		126 // 287: 10%-90%, 126: 50%-50%

struct TimingParameter_65nm{
	static double setup;
	static double clk_q;

	static double BW;
	static double RC;
	static double SA;
	static double ST;
	static double LT;

};

#endif /* TIMING_PARAMETERS_H_ */
