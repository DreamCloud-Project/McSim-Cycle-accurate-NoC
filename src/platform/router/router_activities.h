/*
 * router_activities.h
 *
 *  Created on: Dec 17, 2011
 *      Author: anhttran
 */

#ifndef ROUTER_ACTIVITIES_H_
#define ROUTER_ACTIVITIES_H_

#include "router_parameters.h"

struct BufferActivities{
	int n_rd_wr_cycles;
	int n_rd_only_cycles;
	int n_wr_only_cycles;
	int n_inactive_cycles;
};

struct RoutingActivities{
	int n_head_cycles;
	int n_head_stall_cycles;
	int n_body_cycles;
	int n_inactive_cycles; // including stall cycles

	int n_output_change_cycles;
};

struct CrossbarActivities{
	int n_1_output_cycles;
	int n_2_outputs_cycles;
	int n_3_outputs_cycles;
	int n_4_outputs_cycles;
	int n_5_outputs_cycles;
	int n_inactive_cycles;
};

struct ArbiterActivities{
	int n_active_cycles;
	int n_hold_cycles;
	int n_not_ready_cycles;
	int n_inactive_cycles;

	int n_output_change_cycles;
};

struct OutCreditCounterActivities{
	int n_one_on_cycles;
	int n_one_changes_cycles;
	int n_both_change_diff_cycles;
	int n_inactive_cycles; // including both changes in the same way
};

struct RouterActivities{
	BufferActivities *buffer_activities[N_ROUTER_PORTS];
	RoutingActivities *routing_activities[N_ROUTER_PORTS];
	CrossbarActivities *crossbar_activities;
	ArbiterActivities *arbiter_activities[N_ROUTER_PORTS];
	OutCreditCounterActivities *out_credit_counter_activities[N_ROUTER_PORTS];
};

#endif /* ROUTER_ACTIVITIES_H_ */
