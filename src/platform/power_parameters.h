/*
 * power_parameters.h
 *
 *	Post PR power of each router component at 1.3V, 1GHz in the typical condition
 *	CMOS: Synopsys 90nm, STM 65nm, Nangate 45nm, Synopsys 32nm
 *
 *  Created on: Dec 16, 2011
 *      Author: anhttran
 *
 */

#ifndef POWER_PARAMETERS_H_
#define POWER_PARAMETERS_H_

using namespace std;

//=============== 16-flit buffer---------------------
//#define STM65_POWER_BUFFER_34b_16FLIT_RD_WR 15700 // total = dyn + leak (uW)
#define STM65_POWER_BUFFER_34b_16FLIT_RD_WR 9375 // with clock gating
#define STM65_POWER_BUFFER_34b_16FLIT_RD_WR_CLK 1290 // only clk-gates + clk-trees
#define STM65_POWER_BUFFER_34b_16FLIT_RD_WR_REG 3648 // only registers
//#define STM65_POWER_BUFFER_34b_16FLIT_RD_ONLY 13100

#define STM65_POWER_BUFFER_34b_16FLIT_RD_ONLY 2871 // with clock gating
#define STM65_POWER_BUFFER_34b_16FLIT_RD_ONLY_CLK 100 // rd_counter clk
#define STM65_POWER_BUFFER_34b_16FLIT_RD_ONLY_REG 60	// rd_counter clk
//#define STM65_POWER_BUFFER_34b_16FLIT_WR_ONLY 12800

#define STM65_POWER_BUFFER_34b_16FLIT_WR_ONLY 6554 // rd_wr - rd_only + inactive
#define STM65_POWER_BUFFER_34b_16FLIT_WR_ONLY_CLK 1220
#define STM65_POWER_BUFFER_34b_16FLIT_WR_ONLY_REG 3588

//#define STM65_POWER_BUFFER_34b_16FLIT_INACTIVE 10200

#define STM65_POWER_BUFFER_34b_16FLIT_INACTIVE 50 // due to top-gate + wr_counter gate + rd_counter_gate + leak
#define STM65_POWER_BUFFER_34b_16FLIT_INACTIVE_CLK 30
#define STM65_POWER_BUFFER_34b_16FLIT_INACTIVE_REG 13.12
//#define STM65_POWER_BUFFER_34b_16FLIT_LEAK 18.600 //

#define STM65_POWER_BUFFER_34b_16FLIT_LEAK 20.460 //

//============== 8-flit buffer-----------------
#define STM65_POWER_BUFFER_34b_8FLIT_RD_WR 4561 // with clock gating
#define STM65_POWER_BUFFER_34b_8FLIT_RD_WR_CLK 970
#define STM65_POWER_BUFFER_34b_8FLIT_RD_WR_REG 2112

#define STM65_POWER_BUFFER_34b_8FLIT_RD_ONLY 971 // with clock gating
#define STM65_POWER_BUFFER_34b_8FLIT_RD_ONLY_CLK 82	// for 4-bit rd counter
#define STM65_POWER_BUFFER_34b_8FLIT_RD_ONLY_REG 48

#define STM65_POWER_BUFFER_34b_8FLIT_WR_ONLY 3627 // rd_wr - rd_only + inactive
#define STM65_POWER_BUFFER_34b_8FLIT_WR_ONLY_CLK 918
#define STM65_POWER_BUFFER_34b_8FLIT_WR_ONLY_REG 2064

#define STM65_POWER_BUFFER_34b_8FLIT_INACTIVE 37 // top-gate + wr_counter gate + rd_counter gate + leak
#define STM65_POWER_BUFFER_34b_8FLIT_INACTIVE_CLK 30
#define STM65_POWER_BUFFER_34b_8FLIT_INACTIVE_REG 6.56

#define STM65_POWER_BUFFER_34b_8FLIT_LEAK 6.820 //

//============ 4-flit buffer-------------------
#define STM65_POWER_BUFFER_34b_4FLIT_RD_WR 2888 // with clock gating
#define STM65_POWER_BUFFER_34b_4FLIT_RD_WR_CLK 910
#define STM65_POWER_BUFFER_34b_4FLIT_RD_WR_REG 1344

#define STM65_POWER_BUFFER_34b_4FLIT_RD_ONLY 427 // with clock gating
#define STM65_POWER_BUFFER_34b_4FLIT_RD_ONLY_CLK 64	// 3-bit rd counter
#define STM65_POWER_BUFFER_34b_4FLIT_RD_ONLY_REG 36

#define STM65_POWER_BUFFER_34b_4FLIT_WR_ONLY 2494 // rd_wr - rd_only + inactive
#define STM65_POWER_BUFFER_34b_4FLIT_WR_ONLY_CLK 876
#define STM65_POWER_BUFFER_34b_4FLIT_WR_ONLY_REG 1308

#define STM65_POWER_BUFFER_34b_4FLIT_INACTIVE 33 //
#define STM65_POWER_BUFFER_34b_4FLIT_INACTIVE_CLK 30
#define STM65_POWER_BUFFER_34b_4FLIT_INACTIVE_REG 3.28

#define STM65_POWER_BUFFER_34b_4FLIT_LEAK  3.520 // 2.923 //

//============== 2-flit buffer-----------------------
//#define STM65_POWER_BUFFER_34b_2FLIT_RD_WR 2950
#define STM65_POWER_BUFFER_34b_2FLIT_RD_WR 2021 // no top-clk-tree for 2 gates, 2130
#define STM65_POWER_BUFFER_34b_2FLIT_RD_WR_CLK 720
#define STM65_POWER_BUFFER_34b_2FLIT_RD_WR_REG 960

//#define STM65_POWER_BUFFER_34b_2FLIT_RD_ONLY 1640
#define STM65_POWER_BUFFER_34b_2FLIT_RD_ONLY 249 // 348
#define STM65_POWER_BUFFER_34b_2FLIT_RD_ONLY_CLK 46	// 2-bit rd counter + global-gate + wr_counter-gate
#define STM65_POWER_BUFFER_34b_2FLIT_RD_ONLY_REG 24

//#define STM65_POWER_BUFFER_34b_2FLIT_WR_ONLY 2510
#define STM65_POWER_BUFFER_34b_2FLIT_WR_ONLY 1804 // 1952	// = rd_wr - rd_only + inactive
#define STM65_POWER_BUFFER_34b_2FLIT_WR_ONLY_CLK 704
#define STM65_POWER_BUFFER_34b_2FLIT_WR_ONLY_REG 936

//#define STM65_POWER_BUFFER_34b_2FLIT_INACTIVE 1200
#define STM65_POWER_BUFFER_34b_2FLIT_INACTIVE 32 // 130
#define STM65_POWER_BUFFER_34b_2FLIT_INACTIVE_CLK 30
#define STM65_POWER_BUFFER_34b_2FLIT_INACTIVE_REG 1.64

#define STM65_POWER_BUFFER_34b_2FLIT_LEAK 1.674 // 1.574

//========== 1-flit buffer (in bufferless or circuit-switched routers)-----------
#define STM65_POWER_BUFFER_34b_1FLIT_RD_WR 1479 // no top-clk-tree for 1 gate
#define STM65_POWER_BUFFER_34b_1FLIT_RD_WR_CLK 710
#define STM65_POWER_BUFFER_34b_1FLIT_RD_WR_REG 768

#define STM65_POWER_BUFFER_34b_1FLIT_RD_ONLY 80 // 1-bit rd counter only + wr_clk gate + global-gate
#define STM65_POWER_BUFFER_34b_1FLIT_RD_ONLY_CLK 38
#define STM65_POWER_BUFFER_34b_1FLIT_RD_ONLY_REG 12

#define STM65_POWER_BUFFER_34b_1FLIT_WR_ONLY 1430 // 1952	// = rd_wr - rd_only + inactive
#define STM65_POWER_BUFFER_34b_1FLIT_WR_ONLY_CLK 702
#define STM65_POWER_BUFFER_34b_1FLIT_WR_ONLY_REG 756

#define STM65_POWER_BUFFER_34b_1FLIT_INACTIVE 31 // 130 // due to top-clk-tree
#define STM65_POWER_BUFFER_34b_1FLIT_INACTIVE_CLK 30
#define STM65_POWER_BUFFER_34b_1FLIT_INACTIVE_REG 0.82

#define STM65_POWER_BUFFER_34b_1FLIT_LEAK 0.890 // 1.574

//========= XY routing ----------------
#define STM65_POWER_ROUTING_XY_12b_HEAD 467	// just one time, even the packet stalls
#define STM65_POWER_ROUTING_XY_12b_HEAD_CLK 170 // 5-b output register
#define STM65_POWER_ROUTING_XY_12b_HEAD_REG 120

#define STM65_POWER_ROUTING_XY_12b_HEAD_STALL 10.708	// head stall power = inactive
#define STM65_POWER_ROUTING_XY_12b_HEAD_STALL_CLK 10.0
#define STM65_POWER_ROUTING_XY_12b_HEAD_STALL_REG 0.29

#define STM65_POWER_ROUTING_XY_12b_BODY 17.51
#define STM65_POWER_ROUTING_XY_12b_BODY_CLK 10.0
#define STM65_POWER_ROUTING_XY_12b_BODY_REG 0.29

#define STM65_POWER_ROUTING_XY_12b_INACTIVE 10.708 // also when stalling
#define STM65_POWER_ROUTING_XY_12b_INACTIVE_CLK 10.0
#define STM65_POWER_ROUTING_XY_12b_INACTIVE_REG 0.29

#define STM65_POWER_ROUTING_XY_12b_LEAK 0.708

//======== adaptive routing  ==============
#define STM65_POWER_ROUTING_ADAPTIVE_12b_HEAD 561	// XY + 20%
#define STM65_POWER_ROUTING_ADAPTIVE_12b_HEAD_CLK 170
#define STM65_POWER_ROUTING_ADAPTIVE_12b_HEAD_REG 120

#define STM65_POWER_ROUTING_ADAPTIVE_12b_HEAD_STALL 316.55 // output selection
#define STM65_POWER_ROUTING_ADAPTIVE_12b_HEAD_STALL_CLK 170
#define STM65_POWER_ROUTING_ADAPTIVE_12b_HEAD_STALL_REG 120

#define STM65_POWER_ROUTING_ADAPTIVE_12b_BODY 17.652
#define STM65_POWER_ROUTING_ADAPTIVE_12b_BODY_CLK 10
#define STM65_POWER_ROUTING_ADAPTIVE_12b_BODY_REG 0.29	// leak only

#define STM65_POWER_ROUTING_ADAPTIVE_12b_INACTIVE 10.850 // = leak + clock
#define STM65_POWER_ROUTING_ADAPTIVE_12b_INACTIVE_CLK 10
#define STM65_POWER_ROUTING_ADAPTIVE_12b_INACTIVE_REG 0.29 // leak only

#define STM65_POWER_ROUTING_ADAPTIVE_12b_LEAK 0.850

//======== 4-input arbiter ==================
//#define STM65_POWER_ARBITER_4INPUTS_ACTIVE 443
#define STM65_POWER_ARBITER_4INPUTS_ACTIVE 443 // 543
#define STM65_POWER_ARBITER_4INPUTS_ACTIVE_CLK 170
#define STM65_POWER_ARBITER_4INPUTS_ACTIVE_REG 96

//#define STM65_POWER_ARBITER_4INPUTS_NOT_READY 177
#define STM65_POWER_ARBITER_4INPUTS_NOT_READY 14 // 114
#define STM65_POWER_ARBITER_4INPUTS_NOT_READY_CLK 10
#define STM65_POWER_ARBITER_4INPUTS_NOT_READY_REG 0.097 // leak

//#define STM65_POWER_ARBITER_4INPUTS_HOLD 177
#define STM65_POWER_ARBITER_4INPUTS_HOLD 12 // 112
#define STM65_POWER_ARBITER_4INPUTS_HOLD_CLK 10
#define STM65_POWER_ARBITER_4INPUTS_HOLD_REG 0.097	// leak

//#define STM65_POWER_ARBITER_4INPUTS_INACTIVE 172
#define STM65_POWER_ARBITER_4INPUTS_INACTIVE 10.43 // 110
#define STM65_POWER_ARBITER_4INPUTS_INACTIVE_CLK 10
#define STM65_POWER_ARBITER_4INPUTS_INACTIVE_REG 0.097 // leak

#define STM65_POWER_ARBITER_4INPUTS_LEAK 0.43 // 0.519

//=========== credit out ===============
//#define STM65_POWER_OUT_CREDIT_COUNTER_5b_ONE_ON 338
#define STM65_POWER_OUT_CREDIT_COUNTER_5b_ONE_ON 371 // 471, no top-clk-tree for 1 gate
#define STM65_POWER_OUT_CREDIT_COUNTER_5b_ONE_ON_CLK 170
#define STM65_POWER_OUT_CREDIT_COUNTER_5b_ONE_ON_REG 120

//#define STM65_POWER_OUT_CREDIT_COUNTER_5b_ONE_CHANGES 300
#define STM65_POWER_OUT_CREDIT_COUNTER_5b_ONE_CHANGES 279 // 379
#define STM65_POWER_OUT_CREDIT_COUNTER_5b_ONE_CHANGES_CLK 90
#define STM65_POWER_OUT_CREDIT_COUNTER_5b_ONE_CHANGES_REG 60

//#define STM65_POWER_OUT_CREDIT_COUNTER_5b_BOTH_CHANGE_DIFF 450
#define STM65_POWER_OUT_CREDIT_COUNTER_5b_BOTH_CHANGE_DIFF 483 // 583
#define STM65_POWER_OUT_CREDIT_COUNTER_5b_BOTH_CHANGE_DIFF_CLK 170
#define STM65_POWER_OUT_CREDIT_COUNTER_5b_BOTH_CHANGE_DIFF_REG 120

//#define STM65_POWER_OUT_CREDIT_COUNTER_5b_INACTIVE 185 // = same values
#define STM65_POWER_OUT_CREDIT_COUNTER_5b_INACTIVE 10.349 // 110 // = same values
#define STM65_POWER_OUT_CREDIT_COUNTER_5b_INACTIVE_CLK 10
#define STM65_POWER_OUT_CREDIT_COUNTER_5b_INACTIVE_REG 0.121

#define STM65_POWER_OUT_CREDIT_COUNTER_5b_LEAK 0.349 // 0.401

//============= crossbar (there's no clock and register) ================
#define STM65_POWER_CROSSBAR_34b_ONE_ACTIVE 921 // 701 // N_ACTIVE = ONE_ACTIVE * N
#define STM65_POWER_CROSSBAR_34b_INACTIVE 6.086 // 4.630
#define STM65_POWER_CROSSBAR_34b_LEAK 6.086 // 4.630

//============= inter-router link ===================
#define PTM65_POWER_LINK_1000um_34b_ACTIVE 2112.32 // 1000 um = 1mm
#define PTM65_POWER_LINK_1000um_34b_INACTIVE 5.701
#define PTM65_POWER_LINK_1000um_34b_LEAK 5.701

//#define PTM65_POWER_LINK_500um_34b_ACTIVE 1140.16 // 500 um = 0.5mm
//#define PTM65_POWER_LINK_500um_34b_INACTIVE 5.701
//#define PTM65_POWER_LINK_500um_34b_LEAK 5.701

//============= global clock tree (don't count subtrees inside routers)
#define STM65_POWER_CLOCK_BUFFER_X4 100	// size 4, non-load, active 2 times each cycle
#define STM65_POWER_CLOCK_BUFFER_X8 200
#define STM65_POWER_CLOCK_BUFFER_X16 400
#define STM65_POWER_CLOCK_BUFFER_X32 800
#define STM65_POWER_CLOCK_BUFFER_X64 1600

#define PTM65_POWER_CLOCK_WIRE_1000um_X1 102	// no driver, 1000 um = 1mm, clk signal changes each half cycle
#define PTM65_POWER_CLOCK_WIRE_2000um_X2 292
#define PTM65_POWER_CLOCK_WIRE_4000um_X4 867

#define GENERAL65_POWER_PLL_ACTIVE 4000
#define GENERAL65_POWER_PLL_LEAK 8

//============== data pipeline registers
#define STM65_POWER_PIPELINE_34b_ACTIVE 1319 // no counters
#define STM65_POWER_PIPELINE_34b_ACTIVE_CLK 634
#define STM65_POWER_PIPELINE_34b_ACTIVE_REG 685

#define STM65_POWER_PIPELINE_34b_INACTIVE 10.82
#define STM65_POWER_PIPELINE_34b_INACTIVE_CLK 10	// assuming small leak
#define STM65_POWER_PIPELINE_34b_INACTIVE_REG 0.82	// leak only

#define STM65_POWER_PIPELINE_34b_LEAK 0.82

//============== control pipeline registers
#define STM65_POWER_PIPELINE_5b_ACTIVE 193
#define STM65_POWER_PIPELINE_5b_ACTIVE_CLK 93
#define STM65_POWER_PIPELINE_5b_ACTIVE_REG 100

#define STM65_POWER_PIPELINE_5b_INACTIVE 10.121	// due to clk gate
#define STM65_POWER_PIPELINE_5b_INACTIVE_CLK 10	// assuming small leak
#define STM65_POWER_PIPELINE_5b_INACTIVE_REG 0.121	// leak only

#define STM65_POWER_PIPELINE_5b_LEAK 0.121

struct PowerParameter_90nm{	// Synopsys

};

struct PowerParameter_65nm{	// STM
	static double buffer_34b_rd_wr;
	static double buffer_34b_rd_wr_clk;
	static double buffer_34b_rd_wr_reg;

	static double buffer_34b_rd_only;
	static double buffer_34b_rd_only_clk;
	static double buffer_34b_rd_only_reg;

	static double buffer_34b_wr_only;
	static double buffer_34b_wr_only_clk;
	static double buffer_34b_wr_only_reg;

	static double buffer_34b_inactive;
	static double buffer_34b_inactive_clk;
	static double buffer_34b_inactive_reg;
	static double buffer_34b_leak;

	static double routing_12b_head;
	static double routing_12b_head_clk;
	static double routing_12b_head_reg;

	static double routing_12b_head_stall;
	static double routing_12b_head_stall_clk;
	static double routing_12b_head_stall_reg;

	static double routing_12b_body;
	static double routing_12b_body_clk;
	static double routing_12b_body_reg;

	static double routing_12b_inactive;
	static double routing_12b_inactive_clk;
	static double routing_12b_inactive_reg;
	static double routing_12b_leak;

	static double arbiter_4inputs_active;
	static double arbiter_4inputs_active_clk;
	static double arbiter_4inputs_active_reg;

	static double arbiter_4inputs_hold;
	static double arbiter_4inputs_hold_clk;
	static double arbiter_4inputs_hold_reg;

	static double arbiter_4inputs_not_ready;
	static double arbiter_4inputs_not_ready_clk;
	static double arbiter_4inputs_not_ready_reg;

	static double arbiter_4inputs_inactive;
	static double arbiter_4inputs_inactive_clk;
	static double arbiter_4inputs_inactive_reg;
	static double arbiter_4inputs_leak;

	static double out_credit_counter_5b_one_on;
	static double out_credit_counter_5b_one_on_clk;
	static double out_credit_counter_5b_one_on_reg;

	static double out_credit_counter_5b_one_changes;
	static double out_credit_counter_5b_one_changes_clk;
	static double out_credit_counter_5b_one_changes_reg;

	static double out_credit_counter_5b_both_change_diff;
	static double out_credit_counter_5b_both_change_diff_clk;
	static double out_credit_counter_5b_both_change_diff_reg;

	static double out_credit_counter_5b_inactive; // = both on
	static double out_credit_counter_5b_inactive_clk;
	static double out_credit_counter_5b_inactive_reg;
	static double out_credit_counter_5b_leak;

	static double crossbar_34b_one_active;
	static double crossbar_34b_inactive;
	static double crossbar_34b_leak;

	static double link_1000um_34b_active;
	static double link_1000um_34b_inactive;
	static double link_1000um_34b_leak;

//	static double link_500um_34b_active;
//	static double link_500um_34b_inactive;
//	static double link_500um_34b_leak;

	static double pipeline_34b_active;
	static double pipeline_34b_active_clk;
	static double pipeline_34b_active_reg;

	static double pipeline_34b_inactive;
	static double pipeline_34b_inactive_clk;
	static double pipeline_34b_inactive_reg;

	static double pipeline_34b_leak;

	static double pipeline_5b_active;
	static double pipeline_5b_active_clk;
	static double pipeline_5b_active_reg;

	static double pipeline_5b_inactive;
	static double pipeline_5b_inactive_clk;
	static double pipeline_5b_inactive_reg;

	static double pipeline_5b_leak;


	static double global_clock_tree;

};

#endif /* POWER_PARAMETERS_H_ */
