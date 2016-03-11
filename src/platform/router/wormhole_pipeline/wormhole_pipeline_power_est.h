/*
 * wormhole_pipeline_power_est.h
 *
 *  Created on: Dec 18, 2011
 *      Author: anhttran
 */

#ifndef WORMHOLE_PIPELINE_POWER_EST_H_
#define WORMHOLE_PIPELINE_POWER_EST_H_

#include "../../platform.h"
#include "../../global_variables.h"
#include "../../common_parameters.h"
#include "../../power_parameters.h"
//#include "../router_power_estimation.h"

class WormholePipelinePowerEstimation{
public:
	WormholePipelinePowerEstimation (Platform *_platform);

//	WormholePipelinePowerInfo *get_router_power_info();

	double get_avg_power();	// = clk power + reg power + comb power
	double get_avg_clk_power();
	double get_avg_reg_power();

	double get_avg_leak_power();
	double get_avg_inactive_power(); // = leak power + clking power

	double get_buffer_avg_power();
	double get_buffer_avg_clk_power();
	double get_buffer_avg_reg_power();
	double get_buffer_leak_power();

	double get_routing_avg_power();
	double get_routing_avg_clk_power();
	double get_routing_avg_reg_power();
	double get_routing_leak_power();

	double get_sa_avg_power();
	double get_sa_avg_clk_power();
	double get_sa_avg_reg_power();
	double get_sa_leak_power();

	double get_out_credit_counter_avg_power();
	double get_out_credit_counter_avg_clk_power();
	double get_out_credit_counter_avg_reg_power();
	double get_out_credit_counter_leak_power();

	double get_crossbar_avg_power();
	double get_crossbar_leak_power();

	double get_link_avg_power();
	double get_link_leak_power();

	double get_pipeline_datapath_avg_power();
	double get_pipeline_datapath_avg_clk_power();
	double get_pipeline_datapath_avg_reg_power();
	double get_pipeline_datapath_leak_power();

	double get_pipeline_control_avg_power();
	double get_pipeline_control_avg_clk_power();
	double get_pipeline_control_avg_reg_power();
	double get_pipeline_control_leak_power();

	double get_avg_energy_per_packet();

private:
	Platform *platform;

	double buffer_avg_power; //  = clk + reg + comb
	double buffer_avg_clk_power;
	double buffer_avg_reg_power;
	double buffer_leak_power;
	double buffer_inactive_power;

	double routing_avg_power;
	double routing_avg_clk_power;
	double routing_avg_reg_power;
	double routing_leak_power;
	double routing_inactive_power;

	double sa_avg_power;
	double sa_avg_clk_power;
	double sa_avg_reg_power;
	double sa_leak_power;
	double sa_inactive_power;

	double out_credit_counter_avg_power;
	double out_credit_counter_avg_clk_power;
	double out_credit_counter_avg_reg_power;
	double out_credit_counter_leak_power;
	double out_credit_counter_inactive_power;

	double crossbar_avg_power;
	double crossbar_leak_power;
	double crossbar_inactive_power;

	double link_avg_power;
	double link_leak_power;
	double link_inactive_power;

	double pipeline_datapath_avg_power;
	double pipeline_datapath_avg_clk_power;
	double pipeline_datapath_avg_reg_power;
	double pipeline_datapath_leak_power;
	double pipeline_datapath_inactive_power;

	double pipeline_control_avg_power;
	double pipeline_control_avg_clk_power;
	double pipeline_control_avg_reg_power;
	double pipeline_control_leak_power;
	double pipeline_control_inactive_power;


//	double total_network_power;
//	double total_network_leak_power;

	void get_all_power();
};

#endif /* WORMHOLE_PIPELINE_POWER_EST_H_ */
