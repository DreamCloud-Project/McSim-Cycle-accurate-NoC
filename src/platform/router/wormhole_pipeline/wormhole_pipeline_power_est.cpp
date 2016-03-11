/*
 * wormhole_pipeline_power_est.cpp
 *
 *  Created on: Dec 18, 2011
 *      Author: anhttran
 *
 *  Updated:
 *  	+ 2012/03/27:
 *  		- energy is only proportional to V^2 (not f)
 *  		- link power is proportional with link length (assuming using repeaters)
 */

#include "wormhole_pipeline_power_est.h"

WormholePipelinePowerEstimation::WormholePipelinePowerEstimation(Platform *_platform){
	platform = _platform;

	// get network activities
	get_all_power();
}

void WormholePipelinePowerEstimation::get_all_power(){
	double all_buffer_power = 0;
	double all_buffer_clk_power = 0;
	double all_buffer_reg_power = 0;
//	all_buffer_leak_power = 0;

	double all_routing_power = 0;
	double all_routing_clk_power = 0;
	double all_routing_reg_power = 0;
//	all_routing_leak_power = 0;

	double all_sa_power = 0;
	double all_sa_clk_power = 0;
	double all_sa_reg_power = 0;
//	sa_leak_power = 0;

	double all_out_credit_counter_power = 0;
	double all_out_credit_counter_clk_power = 0;
	double all_out_credit_counter_reg_power = 0;
//	all_out_credit_counter_leak_power = 0;

	double all_crossbar_power = 0;
//	all_crossbar_leak_power = 0;

	double all_link_power = 0;

	double all_pipeline_datapath_power = 0;
	double all_pipeline_datapath_clk_power = 0;
	double all_pipeline_datapath_reg_power = 0;

	double all_pipeline_control_power = 0;
	double all_pipeline_control_clk_power = 0;
	double all_pipeline_control_reg_power = 0;

	// TODO: move all these ratios to the main.cpp
	double link_ratio = RouterParameter::link_length/1000;

	double credit_counter_ratio = (log2((double)RouterParameter::buffer_size)+1)/5;	// default is 5 bits for 16-flit

//	double control_data_ratio = N_ROUTER_PORTS/FLIT_WIDTH; // 5/34

	RouterActivities *router_activities = NULL;

	// TODO: print activities of all routers to a log file
	for (int x=0; x<CommonParameter::dim_x; x++){
		for (int y=0; y<CommonParameter::dim_y; y++){
			router_activities = platform->tile[x][y]->router->getRouterActivities();

			for (int pi=0; pi<N_ROUTER_PORTS; pi++){
				// buffer activities------
				double buffer_rd_wr_percent = (double)(router_activities->buffer_activities[pi]->n_rd_wr_cycles)/(GlobalVariables::last_simulation_time-CommonParameter::warmup_time);
				double buffer_rd_only_percent = (double)(router_activities->buffer_activities[pi]->n_rd_only_cycles)/(GlobalVariables::last_simulation_time-CommonParameter::warmup_time);
				double buffer_wr_only_percent = (double)(router_activities->buffer_activities[pi]->n_wr_only_cycles)/(GlobalVariables::last_simulation_time-CommonParameter::warmup_time);
				double buffer_inactive_percent = (double)(router_activities->buffer_activities[pi]->n_inactive_cycles)/(GlobalVariables::last_simulation_time-CommonParameter::warmup_time);

				// routing activities------
				double routing_head_percent = (double)(router_activities->routing_activities[pi]->n_head_cycles)/(GlobalVariables::last_simulation_time-CommonParameter::warmup_time);
				double routing_head_stall_percent = (double)(router_activities->routing_activities[pi]->n_head_stall_cycles)/(GlobalVariables::last_simulation_time-CommonParameter::warmup_time);
				double routing_body_percent = (double)(router_activities->routing_activities[pi]->n_body_cycles)/(GlobalVariables::last_simulation_time-CommonParameter::warmup_time);
				double routing_inactive_percent = (double)(router_activities->routing_activities[pi]->n_inactive_cycles)/(GlobalVariables::last_simulation_time-CommonParameter::warmup_time);

				double routing_output_change_percent = (double)(router_activities->routing_activities[pi]->n_output_change_cycles)/(GlobalVariables::last_simulation_time-CommonParameter::warmup_time);

				// arbiter activities------
				int po = pi;
				double arbiter_active_percent = (double)(router_activities->arbiter_activities[po]->n_active_cycles)/(GlobalVariables::last_simulation_time-CommonParameter::warmup_time);
				double arbiter_hold_percent = (double)(router_activities->arbiter_activities[po]->n_hold_cycles)/(GlobalVariables::last_simulation_time-CommonParameter::warmup_time);
				double arbiter_not_ready_percent = (double)(router_activities->arbiter_activities[po]->n_not_ready_cycles)/(GlobalVariables::last_simulation_time-CommonParameter::warmup_time);
				double arbiter_inactive_percent = (double)(router_activities->arbiter_activities[po]->n_inactive_cycles)/(GlobalVariables::last_simulation_time-CommonParameter::warmup_time);

				double arbiter_output_change_percent = (double)(router_activities->arbiter_activities[po]->n_output_change_cycles)/(GlobalVariables::last_simulation_time-CommonParameter::warmup_time);

				// out_credit_counter activities-------
				double out_credit_counter_one_on_percent = (double)(router_activities->out_credit_counter_activities[po]->n_one_on_cycles)/(GlobalVariables::last_simulation_time-CommonParameter::warmup_time);
				double out_credit_counter_one_changes_percent = (double)(router_activities->out_credit_counter_activities[po]->n_one_changes_cycles)/(GlobalVariables::last_simulation_time-CommonParameter::warmup_time);
				double out_credit_counter_both_change_diff_percent = (double)(router_activities->out_credit_counter_activities[po]->n_both_change_diff_cycles)/(GlobalVariables::last_simulation_time-CommonParameter::warmup_time);
				double out_credit_counter_inactive_percent = (double)(router_activities->out_credit_counter_activities[po]->n_inactive_cycles)/(GlobalVariables::last_simulation_time-CommonParameter::warmup_time);

				//-------- TEST
				if (x==2 && y==2 && pi==2){
					cout << "==========ACTIVITY CHECKING=========" << endl;
					cout << "Buffer activity[" << pi << "]: " << buffer_rd_wr_percent << "; " << buffer_rd_only_percent << "; " << buffer_wr_only_percent << "; " << buffer_inactive_percent << endl;
					cout << "Routing activity[" << pi << "]: " << routing_head_percent << "; " << routing_head_stall_percent << "; " << routing_body_percent << "; " << routing_inactive_percent << endl;
					cout << "--- output change = " << routing_output_change_percent << endl;
					cout << "Arbiter activity[" << po << "]: " << arbiter_active_percent << "; " << arbiter_hold_percent << "; " << arbiter_not_ready_percent << "; " << arbiter_inactive_percent << endl;
					cout << "--- arbiter output change = " << arbiter_output_change_percent << endl;
					cout << "OutCreditCounter activity[" << po << "]: " << out_credit_counter_one_on_percent << "; " << out_credit_counter_one_changes_percent << "; " << out_credit_counter_both_change_diff_percent << "; " << out_credit_counter_inactive_percent << endl;
				}

				switch (CommonParameter::cmos_process){
				case (90):
						break;
				case (65):
						// buffer power--------
						all_buffer_power += buffer_rd_wr_percent*PowerParameter_65nm::buffer_34b_rd_wr;
						all_buffer_power += buffer_rd_only_percent*PowerParameter_65nm::buffer_34b_rd_only;
						all_buffer_power += buffer_wr_only_percent*PowerParameter_65nm::buffer_34b_wr_only;
						all_buffer_power += buffer_inactive_percent*PowerParameter_65nm::buffer_34b_inactive;

						all_buffer_clk_power += buffer_rd_wr_percent*PowerParameter_65nm::buffer_34b_rd_wr_clk;
						all_buffer_clk_power += buffer_rd_only_percent*PowerParameter_65nm::buffer_34b_rd_only_clk;
						all_buffer_clk_power += buffer_wr_only_percent*PowerParameter_65nm::buffer_34b_wr_only_clk;
						all_buffer_clk_power += buffer_inactive_percent*PowerParameter_65nm::buffer_34b_inactive_clk;

						all_buffer_reg_power += buffer_rd_wr_percent*PowerParameter_65nm::buffer_34b_rd_wr_reg;
						all_buffer_reg_power += buffer_rd_only_percent*PowerParameter_65nm::buffer_34b_rd_only_reg;
						all_buffer_reg_power += buffer_wr_only_percent*PowerParameter_65nm::buffer_34b_wr_only_reg;
						all_buffer_reg_power += buffer_inactive_percent*PowerParameter_65nm::buffer_34b_inactive_reg;


						// routing power-------
						all_routing_power += routing_head_percent*PowerParameter_65nm::routing_12b_head;
						all_routing_power += routing_head_stall_percent*PowerParameter_65nm::routing_12b_head_stall;
						all_routing_power += routing_body_percent*PowerParameter_65nm::routing_12b_body;
						all_routing_power += routing_inactive_percent*PowerParameter_65nm::routing_12b_inactive;

						all_routing_clk_power += routing_head_percent*PowerParameter_65nm::routing_12b_head_clk;
						all_routing_clk_power += routing_head_stall_percent*PowerParameter_65nm::routing_12b_head_stall_clk;
						all_routing_clk_power += routing_body_percent*PowerParameter_65nm::routing_12b_body_clk;
						all_routing_clk_power += routing_inactive_percent*PowerParameter_65nm::routing_12b_inactive_clk;

						all_routing_reg_power += routing_head_percent*PowerParameter_65nm::routing_12b_head_reg;
						all_routing_reg_power += routing_head_stall_percent*PowerParameter_65nm::routing_12b_head_stall_reg;
						all_routing_reg_power += routing_body_percent*PowerParameter_65nm::routing_12b_body_reg;
						all_routing_reg_power += routing_inactive_percent*PowerParameter_65nm::routing_12b_inactive_reg;

						// arbiter power-------
						all_sa_power += arbiter_active_percent*PowerParameter_65nm::arbiter_4inputs_active;
						all_sa_power += arbiter_hold_percent*PowerParameter_65nm::arbiter_4inputs_hold;
						all_sa_power += arbiter_not_ready_percent*PowerParameter_65nm::arbiter_4inputs_not_ready;
						all_sa_power += arbiter_inactive_percent*PowerParameter_65nm::arbiter_4inputs_inactive;

						all_sa_clk_power += arbiter_active_percent*PowerParameter_65nm::arbiter_4inputs_active_clk;
						all_sa_clk_power += arbiter_hold_percent*PowerParameter_65nm::arbiter_4inputs_hold_clk;
						all_sa_clk_power += arbiter_not_ready_percent*PowerParameter_65nm::arbiter_4inputs_not_ready_clk;
						all_sa_clk_power += arbiter_inactive_percent*PowerParameter_65nm::arbiter_4inputs_inactive_clk;

						all_sa_reg_power += arbiter_active_percent*PowerParameter_65nm::arbiter_4inputs_active_reg;
						all_sa_reg_power += arbiter_hold_percent*PowerParameter_65nm::arbiter_4inputs_hold_reg;
						all_sa_reg_power += arbiter_not_ready_percent*PowerParameter_65nm::arbiter_4inputs_not_ready_reg;
						all_sa_reg_power += arbiter_inactive_percent*PowerParameter_65nm::arbiter_4inputs_inactive_reg;


						// out_credit_counter power------
						all_out_credit_counter_power += out_credit_counter_one_on_percent * PowerParameter_65nm::out_credit_counter_5b_one_on * credit_counter_ratio;
						all_out_credit_counter_power += out_credit_counter_one_changes_percent * PowerParameter_65nm::out_credit_counter_5b_one_changes * credit_counter_ratio;
						all_out_credit_counter_power += out_credit_counter_both_change_diff_percent * PowerParameter_65nm::out_credit_counter_5b_both_change_diff * credit_counter_ratio;
						all_out_credit_counter_power += out_credit_counter_inactive_percent * PowerParameter_65nm::out_credit_counter_5b_inactive * credit_counter_ratio;

						all_out_credit_counter_clk_power += out_credit_counter_one_on_percent * PowerParameter_65nm::out_credit_counter_5b_one_on_clk * credit_counter_ratio;
						all_out_credit_counter_clk_power += out_credit_counter_one_changes_percent * PowerParameter_65nm::out_credit_counter_5b_one_changes_clk * credit_counter_ratio;
						all_out_credit_counter_clk_power += out_credit_counter_both_change_diff_percent * PowerParameter_65nm::out_credit_counter_5b_both_change_diff_clk * credit_counter_ratio;
						all_out_credit_counter_clk_power += out_credit_counter_inactive_percent * PowerParameter_65nm::out_credit_counter_5b_inactive_clk * credit_counter_ratio;

						all_out_credit_counter_reg_power += out_credit_counter_one_on_percent * PowerParameter_65nm::out_credit_counter_5b_one_on_reg * credit_counter_ratio;
						all_out_credit_counter_reg_power += out_credit_counter_one_changes_percent * PowerParameter_65nm::out_credit_counter_5b_one_changes_reg * credit_counter_ratio;
						all_out_credit_counter_reg_power += out_credit_counter_both_change_diff_percent * PowerParameter_65nm::out_credit_counter_5b_both_change_diff_reg * credit_counter_ratio;
						all_out_credit_counter_reg_power += out_credit_counter_inactive_percent * PowerParameter_65nm::out_credit_counter_5b_inactive_reg * credit_counter_ratio;

						// pipeline power--------
						// including output buffer pipeline
						if ( ((RouterParameter::pipeline_type == PIPELINE_TYPE_2_1)
								|| (RouterParameter::pipeline_type == PIPELINE_TYPE_2_2)
								|| (RouterParameter::n_pipeline_stages >= 3))
								&& (RouterParameter::buffer_size > 1) ){	// only having output mux if bsize>=2
							all_pipeline_datapath_power += (buffer_rd_wr_percent + buffer_rd_only_percent) * PowerParameter_65nm::pipeline_34b_active;
							all_pipeline_datapath_power += (buffer_wr_only_percent + buffer_inactive_percent) * PowerParameter_65nm::pipeline_34b_inactive;

							all_pipeline_datapath_clk_power += (buffer_rd_wr_percent + buffer_rd_only_percent) * PowerParameter_65nm::pipeline_34b_active_clk;
							all_pipeline_datapath_clk_power += (buffer_wr_only_percent + buffer_inactive_percent) * PowerParameter_65nm::pipeline_34b_inactive_clk;

							all_pipeline_datapath_reg_power += (buffer_rd_wr_percent + buffer_rd_only_percent) * PowerParameter_65nm::pipeline_34b_active_reg;
							all_pipeline_datapath_reg_power += (buffer_wr_only_percent + buffer_inactive_percent) * PowerParameter_65nm::pipeline_34b_inactive_reg;
						}

						// including control pipeline at the SA output
						if ((RouterParameter::pipeline_type == PIPELINE_TYPE_2_2)
							|| (RouterParameter::pipeline_type == PIPELINE_TYPE_3_1)
							|| (RouterParameter::pipeline_type == PIPELINE_TYPE_3_3)
							|| (RouterParameter::pipeline_type == PIPELINE_TYPE_4)){
							// when hold, SA doesn't change --> inactive; but each time not_ready changes, SA changes --> active
							// once not_ready goes high, it will goes low --> SA switching = 2x not_ready goes high
							all_pipeline_control_power += arbiter_output_change_percent * PowerParameter_65nm::pipeline_5b_active;
							all_pipeline_control_power += (1-arbiter_output_change_percent) * PowerParameter_65nm::pipeline_5b_inactive;

							all_pipeline_control_clk_power += arbiter_output_change_percent * PowerParameter_65nm::pipeline_5b_active_clk;
							all_pipeline_control_clk_power += (1-arbiter_output_change_percent) * PowerParameter_65nm::pipeline_5b_inactive_clk;

							all_pipeline_control_reg_power += arbiter_output_change_percent * PowerParameter_65nm::pipeline_5b_active_reg;
							all_pipeline_control_reg_power += (1-arbiter_output_change_percent) * PowerParameter_65nm::pipeline_5b_inactive_reg;
						}

/*
						// including control pipeline at the RC output
						if (RouterParameter::n_pipeline_stages == 5){
							all_pipeline_control_power += routing_output_change_percent * PowerParameter_65nm::pipeline_5b_active;
							all_pipeline_control_power += (1-routing_output_change_percent) * PowerParameter_65nm::pipeline_5b_inactive;

							all_pipeline_control_clk_power += routing_output_change_percent * PowerParameter_65nm::pipeline_5b_active_clk;
							all_pipeline_control_clk_power += (1-routing_output_change_percent) * PowerParameter_65nm::pipeline_5b_inactive_clk;

							all_pipeline_control_reg_power += routing_output_change_percent * PowerParameter_65nm::pipeline_5b_active_reg;
							all_pipeline_control_reg_power += (1-routing_output_change_percent) * PowerParameter_65nm::pipeline_5b_inactive_reg;
						}
*/
						break;
				case (45):
						break;
				case (32):
						break;
				case (22):
						break;
				default:;
				}
			}

			// crossbar activities------
			double xbar_1_output_percent = (double)(router_activities->crossbar_activities->n_1_output_cycles)/(GlobalVariables::last_simulation_time-CommonParameter::warmup_time);
			double xbar_2_outputs_percent = (double)(router_activities->crossbar_activities->n_2_outputs_cycles)/(GlobalVariables::last_simulation_time-CommonParameter::warmup_time);
			double xbar_3_outputs_percent = (double)(router_activities->crossbar_activities->n_3_outputs_cycles)/(GlobalVariables::last_simulation_time-CommonParameter::warmup_time);
			double xbar_4_outputs_percent = (double)(router_activities->crossbar_activities->n_4_outputs_cycles)/(GlobalVariables::last_simulation_time-CommonParameter::warmup_time);
			double xbar_5_outputs_percent = (double)(router_activities->crossbar_activities->n_5_outputs_cycles)/(GlobalVariables::last_simulation_time-CommonParameter::warmup_time);
			double xbar_inactive_percent = (double)(router_activities->crossbar_activities->n_inactive_cycles)/(GlobalVariables::last_simulation_time-CommonParameter::warmup_time);

			// TESTING...
			if (x==2 && y==2){
				cout << "Crossbar activity: " << xbar_1_output_percent << "; " << xbar_2_outputs_percent << "; " << xbar_3_outputs_percent << "; " << xbar_4_outputs_percent << "; " << xbar_5_outputs_percent << "; " << xbar_inactive_percent << endl;
			}

			switch (CommonParameter::cmos_process){
			case (90):
					break;
			case (65):
					// crossbar power-----------
					all_crossbar_power += xbar_1_output_percent * PowerParameter_65nm::crossbar_34b_one_active;
					all_crossbar_power += xbar_2_outputs_percent * PowerParameter_65nm::crossbar_34b_one_active * 2;
					all_crossbar_power += xbar_3_outputs_percent * PowerParameter_65nm::crossbar_34b_one_active * 3;
					all_crossbar_power += xbar_4_outputs_percent * PowerParameter_65nm::crossbar_34b_one_active * 4;
					all_crossbar_power += xbar_5_outputs_percent * PowerParameter_65nm::crossbar_34b_one_active * 5;
					all_crossbar_power += xbar_inactive_percent * PowerParameter_65nm::crossbar_34b_inactive;

					// link power (having the same activity as crossbar)----------
					all_link_power += xbar_1_output_percent * PowerParameter_65nm::link_1000um_34b_active * link_ratio;
					all_link_power += xbar_2_outputs_percent * PowerParameter_65nm::link_1000um_34b_active * 2 * link_ratio;
					all_link_power += xbar_3_outputs_percent * PowerParameter_65nm::link_1000um_34b_active * 3 * link_ratio;
					all_link_power += xbar_4_outputs_percent * PowerParameter_65nm::link_1000um_34b_active * 4 * link_ratio;
					all_link_power += xbar_5_outputs_percent * PowerParameter_65nm::link_1000um_34b_active * 5 * link_ratio;
//					all_link_power *= RouterParameter::link_length/1000;	// dynamic link power is proportional to wire length
					all_link_power += xbar_inactive_percent * PowerParameter_65nm::link_1000um_34b_inactive * N_ROUTER_PORTS * link_ratio;

					// accumulated pipeline power (link) ----------------
					if ((RouterParameter::pipeline_type == PIPELINE_TYPE_2_3)
							|| (RouterParameter::pipeline_type == PIPELINE_TYPE_3_2)
							|| (RouterParameter::pipeline_type == PIPELINE_TYPE_3_3)
							|| (RouterParameter::pipeline_type == PIPELINE_TYPE_4)){ // including link pipeline
						all_pipeline_datapath_power += xbar_1_output_percent * PowerParameter_65nm::pipeline_34b_active;
						all_pipeline_datapath_power += xbar_2_outputs_percent * PowerParameter_65nm::pipeline_34b_active * 2;
						all_pipeline_datapath_power += xbar_3_outputs_percent * PowerParameter_65nm::pipeline_34b_active * 3;
						all_pipeline_datapath_power += xbar_4_outputs_percent * PowerParameter_65nm::pipeline_34b_active * 4;
						all_pipeline_datapath_power += xbar_5_outputs_percent * PowerParameter_65nm::pipeline_34b_active * 5;
						all_pipeline_datapath_power += xbar_inactive_percent * PowerParameter_65nm::pipeline_34b_inactive * N_ROUTER_PORTS;

						all_pipeline_datapath_clk_power += xbar_1_output_percent * PowerParameter_65nm::pipeline_34b_active_clk;
						all_pipeline_datapath_clk_power += xbar_2_outputs_percent * PowerParameter_65nm::pipeline_34b_active_clk * 2;
						all_pipeline_datapath_clk_power += xbar_3_outputs_percent * PowerParameter_65nm::pipeline_34b_active_clk * 3;
						all_pipeline_datapath_clk_power += xbar_4_outputs_percent * PowerParameter_65nm::pipeline_34b_active_clk * 4;
						all_pipeline_datapath_clk_power += xbar_5_outputs_percent * PowerParameter_65nm::pipeline_34b_active_clk * 5;
						all_pipeline_datapath_clk_power += xbar_inactive_percent * PowerParameter_65nm::pipeline_34b_inactive_clk * N_ROUTER_PORTS;

						all_pipeline_datapath_reg_power += xbar_1_output_percent * PowerParameter_65nm::pipeline_34b_active_reg;
						all_pipeline_datapath_reg_power += xbar_2_outputs_percent * PowerParameter_65nm::pipeline_34b_active_reg * 2;
						all_pipeline_datapath_reg_power += xbar_3_outputs_percent * PowerParameter_65nm::pipeline_34b_active_reg * 3;
						all_pipeline_datapath_reg_power += xbar_4_outputs_percent * PowerParameter_65nm::pipeline_34b_active_reg * 4;
						all_pipeline_datapath_reg_power += xbar_5_outputs_percent * PowerParameter_65nm::pipeline_34b_active_reg * 5;
						all_pipeline_datapath_reg_power += xbar_inactive_percent * PowerParameter_65nm::pipeline_34b_inactive_reg * N_ROUTER_PORTS;
					}


					break;
			case (45):
					break;
			case (32):
					break;
			case (22):
					break;
			default:;
			}
		}
	}

	delete (router_activities);

	//------- average power
	buffer_avg_power = all_buffer_power/(CommonParameter::dim_x * CommonParameter::dim_y);
	buffer_avg_clk_power = all_buffer_clk_power/(CommonParameter::dim_x * CommonParameter::dim_y);
	buffer_avg_reg_power = all_buffer_reg_power/(CommonParameter::dim_x * CommonParameter::dim_y);

	routing_avg_power = all_routing_power/(CommonParameter::dim_x * CommonParameter::dim_y);
	routing_avg_clk_power = all_routing_clk_power/(CommonParameter::dim_x * CommonParameter::dim_y);
	routing_avg_reg_power = all_routing_reg_power/(CommonParameter::dim_x * CommonParameter::dim_y);

	sa_avg_power = all_sa_power/(CommonParameter::dim_x * CommonParameter::dim_y);
	sa_avg_clk_power = all_sa_clk_power/(CommonParameter::dim_x * CommonParameter::dim_y);
	sa_avg_reg_power = all_sa_reg_power/(CommonParameter::dim_x * CommonParameter::dim_y);

	out_credit_counter_avg_power = all_out_credit_counter_power/(CommonParameter::dim_x * CommonParameter::dim_y);
	out_credit_counter_avg_clk_power = all_out_credit_counter_clk_power/(CommonParameter::dim_x * CommonParameter::dim_y);
	out_credit_counter_avg_reg_power = all_out_credit_counter_reg_power/(CommonParameter::dim_x * CommonParameter::dim_y);

	crossbar_avg_power = all_crossbar_power/(CommonParameter::dim_x * CommonParameter::dim_y);

	link_avg_power = all_link_power/(CommonParameter::dim_x * CommonParameter::dim_y);

	pipeline_datapath_avg_power = all_pipeline_datapath_power/(CommonParameter::dim_x * CommonParameter::dim_y);
	pipeline_datapath_avg_clk_power = all_pipeline_datapath_clk_power/(CommonParameter::dim_x * CommonParameter::dim_y);
	pipeline_datapath_avg_reg_power = all_pipeline_datapath_reg_power/(CommonParameter::dim_x * CommonParameter::dim_y);

	pipeline_control_avg_power = all_pipeline_control_power/(CommonParameter::dim_x * CommonParameter::dim_y);
	pipeline_control_avg_clk_power = all_pipeline_control_clk_power/(CommonParameter::dim_x * CommonParameter::dim_y);
	pipeline_control_avg_reg_power = all_pipeline_control_reg_power/(CommonParameter::dim_x * CommonParameter::dim_y);


	//------- average leakage and inactive (idle) power
	buffer_leak_power = 0;
	buffer_inactive_power = 0;

	routing_leak_power = 0;
	routing_inactive_power = 0;

	sa_leak_power = 0;
	sa_inactive_power = 0;

	out_credit_counter_leak_power = 0;
	out_credit_counter_inactive_power = 0;

	crossbar_leak_power = 0;
	crossbar_inactive_power = 0;

	link_leak_power = 0;
	link_inactive_power = 0;

	pipeline_datapath_leak_power = 0;
	pipeline_datapath_inactive_power = 0;

	pipeline_control_leak_power = 0;
	pipeline_control_inactive_power = 0;

	switch (CommonParameter::cmos_process){
	case (90):
			break;
	case (65):
			// buffer leakage power
			buffer_leak_power += PowerParameter_65nm::buffer_34b_leak * N_ROUTER_PORTS;
			buffer_inactive_power += PowerParameter_65nm::buffer_34b_inactive * N_ROUTER_PORTS;

			// routing leak power
			routing_leak_power += PowerParameter_65nm::routing_12b_leak * N_ROUTER_PORTS;
			routing_inactive_power += PowerParameter_65nm::routing_12b_inactive * N_ROUTER_PORTS;

			// sa leak power
			sa_leak_power += PowerParameter_65nm::arbiter_4inputs_leak * N_ROUTER_PORTS;
			sa_inactive_power += PowerParameter_65nm::arbiter_4inputs_inactive * N_ROUTER_PORTS;

			// out_credit_counter leak power
			out_credit_counter_leak_power += PowerParameter_65nm::out_credit_counter_5b_leak * N_ROUTER_PORTS * credit_counter_ratio;
			out_credit_counter_inactive_power += PowerParameter_65nm::out_credit_counter_5b_inactive * N_ROUTER_PORTS * credit_counter_ratio;

			// crossbar leak power
			crossbar_leak_power += PowerParameter_65nm::crossbar_34b_leak;
			crossbar_inactive_power += PowerParameter_65nm::crossbar_34b_inactive;

			// TODO: if long wires use repeater, leak and inactive also depend on wire length
			link_leak_power += PowerParameter_65nm::link_1000um_34b_leak * N_ROUTER_PORTS * link_ratio;
			link_inactive_power += PowerParameter_65nm::link_1000um_34b_inactive * N_ROUTER_PORTS * link_ratio;

			// pipeline leak power
			// adding link pipeline
			if ((RouterParameter::pipeline_type == PIPELINE_TYPE_2_3)
					|| (RouterParameter::pipeline_type == PIPELINE_TYPE_3_2)
					|| (RouterParameter::pipeline_type == PIPELINE_TYPE_3_3)
					|| (RouterParameter::pipeline_type == PIPELINE_TYPE_4)){
				pipeline_datapath_leak_power += PowerParameter_65nm::pipeline_34b_leak * N_ROUTER_PORTS;
				pipeline_datapath_inactive_power += PowerParameter_65nm::pipeline_34b_inactive * N_ROUTER_PORTS;
			}

			// adding output buffer pipeline
			if ( ((RouterParameter::pipeline_type == PIPELINE_TYPE_2_1)
					|| (RouterParameter::pipeline_type == PIPELINE_TYPE_2_2)
					|| (RouterParameter::n_pipeline_stages >= 3))
				&& (RouterParameter::buffer_size>1) ){
				pipeline_datapath_leak_power += PowerParameter_65nm::pipeline_34b_leak * N_ROUTER_PORTS;
				pipeline_datapath_inactive_power += PowerParameter_65nm::pipeline_34b_inactive * N_ROUTER_PORTS;
			}

			// adding SA output pipeline
			if ((RouterParameter::pipeline_type == PIPELINE_TYPE_2_2)
					|| (RouterParameter::pipeline_type == PIPELINE_TYPE_3_1)
					|| (RouterParameter::pipeline_type == PIPELINE_TYPE_3_3)
					|| (RouterParameter::pipeline_type == PIPELINE_TYPE_4)){
				pipeline_control_leak_power += PowerParameter_65nm::pipeline_5b_leak * N_ROUTER_PORTS;
				pipeline_control_inactive_power += PowerParameter_65nm::pipeline_5b_inactive * N_ROUTER_PORTS;
			}
/*
			if (RouterParameter::n_pipeline_stages == 5){ // adding RC output pipeline
				pipeline_control_leak_power += PowerParameter_65nm::pipeline_5b_leak * N_ROUTER_PORTS;
				pipeline_control_inactive_power += PowerParameter_65nm::pipeline_5b_inactive * N_ROUTER_PORTS;
			}
*/

			break;
	case (45):
			break;
	case (32):
			break;
	case (22):
			break;
	default:;
	}

}

/*
WormholePipelinePowerInfo *WormholePipelinePowerEstimation::get_router_power_info(){
	WormholePipelinePowerInfo *router_power_info = new WormholePipelinePowerInfo();

	router_power_info->buffer_total_power = all_buffer_total_power/(CommonParameter::dim_x * CommonParameter::dim_y);;
	router_power_info->buffet_leak_power = 0;

	router_power_info->routing_total_power = all_routing_total_power/(CommonParameter::dim_x * CommonParameter::dim_y);;
	router_power_info->routing_leak_power = 0;

	router_power_info->sa_total_power = all_sa_total_power/(CommonParameter::dim_x * CommonParameter::dim_y);;
	router_power_info->sa_leak_power = 0;

	router_power_info->crossbar_total_power = all_crossbar_total_power/(CommonParameter::dim_x * CommonParameter::dim_y);;
	router_power_info->crossbar_leak_power = 0;

	router_power_info->out_credit_counter_total_power = all_out_credit_counter_total_power/(CommonParameter::dim_x * CommonParameter::dim_y);;
	router_power_info->out_credit_counter_leak_power = 0;

	return router_power_info;
}
*/

/*
 * get_avg_power
 */
double WormholePipelinePowerEstimation::get_avg_power(){
	double avg_power = 0;

	avg_power += buffer_avg_power;
	avg_power += routing_avg_power;
	avg_power += sa_avg_power;
	avg_power += crossbar_avg_power;
	avg_power += out_credit_counter_avg_power;
	avg_power += link_avg_power;

	avg_power += pipeline_datapath_avg_power;
	avg_power += pipeline_control_avg_power;

	return avg_power;
}

/*
 * get_avg_clk_power
 */
double WormholePipelinePowerEstimation::get_avg_clk_power(){
	double avg_clk_power = 0;

	avg_clk_power += buffer_avg_clk_power;
	avg_clk_power += routing_avg_clk_power;
	avg_clk_power += sa_avg_clk_power;
//	avg_clk_power += crossbar_avg_clk_power;
	avg_clk_power += out_credit_counter_avg_clk_power;
//	avg_clk_power += link_avg_clk_power;

	avg_clk_power += pipeline_datapath_avg_clk_power;
	avg_clk_power += pipeline_control_avg_clk_power;

	return avg_clk_power;
}


/*
 * get_avg_reg_power
 */
double WormholePipelinePowerEstimation::get_avg_reg_power(){
	double avg_reg_power = 0;

	avg_reg_power += buffer_avg_reg_power;
	avg_reg_power += routing_avg_reg_power;
	avg_reg_power += sa_avg_reg_power;
//	avg_reg_power += crossbar_avg_reg_power;
	avg_reg_power += out_credit_counter_avg_reg_power;
//	avg_reg_power += link_avg_reg_power;

	avg_reg_power += pipeline_datapath_avg_reg_power;
	avg_reg_power += pipeline_control_avg_reg_power;

	return avg_reg_power;
}


/*
 * get_avg_leak_power
 */
double WormholePipelinePowerEstimation::get_avg_leak_power(){
	double avg_leak_power = 0;

	avg_leak_power += buffer_leak_power;
	avg_leak_power += routing_leak_power;
	avg_leak_power += sa_leak_power;
	avg_leak_power += crossbar_leak_power;
	avg_leak_power += out_credit_counter_leak_power;
	avg_leak_power += link_leak_power;

	avg_leak_power += pipeline_datapath_leak_power;
	avg_leak_power += pipeline_control_leak_power;

	return avg_leak_power;
}


/*
 * get_avg_leak_power
 */
double WormholePipelinePowerEstimation::get_avg_inactive_power(){
	double avg_inactive_power = 0;

	avg_inactive_power += buffer_inactive_power;
	avg_inactive_power += routing_inactive_power;
	avg_inactive_power += sa_inactive_power;
	avg_inactive_power += crossbar_inactive_power;
	avg_inactive_power += out_credit_counter_inactive_power;
	avg_inactive_power += link_inactive_power;

	avg_inactive_power += pipeline_datapath_inactive_power;
	avg_inactive_power += pipeline_control_inactive_power;

	return avg_inactive_power;
}


/*
 * get_buffer_avg_power
 */
double WormholePipelinePowerEstimation::get_buffer_avg_power(){
	return buffer_avg_power;
}

/*
 * get_buffer_avg_clk_power
 */
double WormholePipelinePowerEstimation::get_buffer_avg_clk_power(){
	return buffer_avg_clk_power;
}

/*
 * get_buffer_avg_reg_power
 */
double WormholePipelinePowerEstimation::get_buffer_avg_reg_power(){
	return buffer_avg_reg_power;
}

/*
 * get_buffer_leak_power
 */
double WormholePipelinePowerEstimation::get_buffer_leak_power(){
	return buffer_leak_power;
}

/*
 * get_routing_avg_power
 */
double WormholePipelinePowerEstimation::get_routing_avg_power(){
	return routing_avg_power;
}

/*
 * get_routing_avg_clk_power
 */
double WormholePipelinePowerEstimation::get_routing_avg_clk_power(){
	return routing_avg_clk_power;
}

/*
 * get_routing_avg_reg_power
 */
double WormholePipelinePowerEstimation::get_routing_avg_reg_power(){
	return routing_avg_reg_power;
}


/*
 * get_routing_leak_power
 */
double WormholePipelinePowerEstimation::get_routing_leak_power(){
	return routing_leak_power;
}


/*
 * get_sa_avg_power
 */
double WormholePipelinePowerEstimation::get_sa_avg_power(){
	return sa_avg_power;
}

/*
 * get_sa_avg_clk_power
 */
double WormholePipelinePowerEstimation::get_sa_avg_clk_power(){
	return sa_avg_clk_power;
}

/*
 * get_sa_avg_reg_power
 */
double WormholePipelinePowerEstimation::get_sa_avg_reg_power(){
	return sa_avg_reg_power;
}

/*
 * get_sa_leak_power
 */
double WormholePipelinePowerEstimation::get_sa_leak_power(){
	return sa_leak_power;
}


/*
 * get_crossbar_avg_power
 */
double WormholePipelinePowerEstimation::get_crossbar_avg_power(){
	return crossbar_avg_power;
}

/*
 * get_crossbar_leak_power
 */
double WormholePipelinePowerEstimation::get_crossbar_leak_power(){
	return crossbar_leak_power;
}


/*
 * get_out_credit_counter_power
 */
double WormholePipelinePowerEstimation::get_out_credit_counter_avg_power(){
	return out_credit_counter_avg_power;
}

/*
 * get_out_credit_counter_clk_power
 */
double WormholePipelinePowerEstimation::get_out_credit_counter_avg_clk_power(){
	return out_credit_counter_avg_clk_power;
}

/*
 * get_out_credit_counter_reg_power
 */
double WormholePipelinePowerEstimation::get_out_credit_counter_avg_reg_power(){
	return out_credit_counter_avg_reg_power;
}


/*
 * get_out_credit_counter_leak_power
 */
double WormholePipelinePowerEstimation::get_out_credit_counter_leak_power(){
	return out_credit_counter_leak_power;
}

/*
 * get_link_avg_power
 */
double WormholePipelinePowerEstimation::get_link_avg_power(){
	return link_avg_power;
}

/*
 * get_link_leak_power
 */
double WormholePipelinePowerEstimation::get_link_leak_power(){
	return link_leak_power;
}

/*
 * get_pipeline_datapath_avg_power
 */
double WormholePipelinePowerEstimation::get_pipeline_datapath_avg_power(){
	return pipeline_datapath_avg_power;
}

/*
 * get_pipeline_datapath_avg_clk_power
 */
double WormholePipelinePowerEstimation::get_pipeline_datapath_avg_clk_power(){
	return pipeline_datapath_avg_clk_power;
}

/*
 * get_pipeline_datapath_avg_reg_power
 */
double WormholePipelinePowerEstimation::get_pipeline_datapath_avg_reg_power(){
	return pipeline_datapath_avg_reg_power;
}

/*
 * get_pipeline_datapath_leak_power
 */
double WormholePipelinePowerEstimation::get_pipeline_datapath_leak_power(){
	return pipeline_datapath_leak_power;
}

/*
 * get_pipeline_control_avg_power
 */
double WormholePipelinePowerEstimation::get_pipeline_control_avg_power(){
	return pipeline_control_avg_power;
}

/*
 * get_pipeline_control_avg_clk_power
 */
double WormholePipelinePowerEstimation::get_pipeline_control_avg_clk_power(){
	return pipeline_control_avg_clk_power;
}

/*
 * get_pipeline_control_avg_reg_power
 */
double WormholePipelinePowerEstimation::get_pipeline_control_avg_reg_power(){
	return pipeline_control_avg_reg_power;
}

/*
 * get_pipeline_control_leak_power
 */
double WormholePipelinePowerEstimation::get_pipeline_control_leak_power(){
	return pipeline_control_leak_power;
}

/*
 * 	average packet energy at 1GHz (Tclk = 1ns = 10^-9 s)
 */
double WormholePipelinePowerEstimation::get_avg_energy_per_packet(){
	double avg_packet_energy = (get_avg_power()/CommonParameter::operating_clk_freq)
									* (GlobalVariables::last_simulation_time-CommonParameter::warmup_time)
									/GlobalVariables::n_total_rx_packets; // (pJ/packet)

	return avg_packet_energy; // (fJ/packet)
}
