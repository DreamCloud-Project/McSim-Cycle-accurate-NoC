//============================================================================
// Name        : main.cpp
// Author      : Anh Tran, Bevan Baas
// Version     : v1.0
// Copyright   : (c) by VCL Lab, ECE Department, UC Davis
// Description : NoCTweak - an On-Chip Network Simulator
// Created on: Apr 22, 2010
//
//  Please send an email to anhtrandavis@gmail.com to ask for your permission
//  before using this code;
//  and please cite the below paper in your related publications:
//	Anh Tran and Bevan Baas,
//	"NoCTweak: a Highly Parameterizable Simulator for Early Exploration of
//	Performance and Energy of Networks On-Chip," Technical Report, VCL Lab,
//	ECE Department, UC Davis, July 2012.
//
//============================================================================

#include <ctime>
#include <iostream>
#include <math.h>
#include "common_functions.h"
#include "console_parse_show.h"
#include "platform.h"
#include "proc/dreamcloud/dc_platform.h"
#include "proc/embedded/embedded_app_info.h"
#include "proc/synthetic/synthetic_evaluation.h"
#include "proc/embedded/embedded_evaluation.h"
#include "router/wormhole_pipeline/wormhole_pipeline_power_est.h"

//#include "cmdlineparser.h"
#include "global_variables.h"

using namespace std;

//--------- initialize global variables
int GlobalVariables::n_total_rx_packets = 0;

int GlobalVariables::last_simulation_time = 0;

//----------- setup all default values for all parameters
int CommonParameter::sim_mode = SIM_MODE_CYCLE;

int CommonParameter::simulation_time = SIMULATION_TIME;
int CommonParameter::warmup_time = WARMUP_TIME;

int CommonParameter::max_n_rx_packets = MAX_N_RX_PACKETS;

int CommonParameter::dim_x = 4;
int CommonParameter::dim_y = 4;

int CommonParameter::random_seed = time(NULL);

bool CommonParameter::is_vcd_traced = true;
string CommonParameter::vcd_filename = "waveform";

int CommonParameter::platform_type = PLATFORM_DC;

//----------- router parameters default
double RouterParameter::link_length = 1000; // 1000 um

int RouterParameter::router_type = ROUTER_VC_RT;

int RouterParameter::n_VCs = NUM_VCS;

int RouterParameter::n_SBs = NUM_SBs;

int RouterParameter::data_link_width = DATA_LINK_WIDTH;

unsigned int RouterParameter::buffer_size = BUFFER_SIZE;
//unsigned int RouterParameter::buffer_size = 4;

unsigned int RouterParameter::shared_buffer_size = SHARED_BUFFER_SIZE;
int RouterParameter::buffer_reserve = BUFFER_RESERVE;
int RouterParameter::vc_buffer_th_off = VC_BUFFER_TH_OFF;
int RouterParameter::vc_buffer_th_on = VC_BUFFER_TH_ON;

int RouterParameter::routing_type = ROUTING_MINIMAL_NEGATIVE_FIRST;
int RouterParameter::output_channel_select = OUTPUT_CHANNEL_SELECT_XY_ORDERED;

int RouterParameter::packet_length_aware = 1;

int RouterParameter::arbiter_type = ARBITER_ROUNDROBIN;
int RouterParameter::vca_type = VCA_ROUNDROBIN;
int RouterParameter::sa_type = SA_ROUNDROBIN;
int RouterParameter::sba_type = SBA_ROUNDROBIN;

int RouterParameter::sba_threshold = SBA_THRESHOLD;
bool RouterParameter::sba_regulated = SBA_REGULATED;

int RouterParameter::pipeline_type = PIPELINE_TYPE_4;
int RouterParameter::n_pipeline_stages = 4;

//------- Processor parameters default
int ProcessorParameters::packet_length_type = PACKET_LENGTH_TYPE_FIXED;

int ProcessorParameters::packet_length = PACKET_LENGTH;

int ProcessorParameters::min_packet_length = 2;
int ProcessorParameters::max_packet_length = 10;

double ProcessorParameters::flit_inject_rate = FLIT_INJECTION_RATE;

int ProcessorParameters::inter_arrival_time_type = INTER_ARRIVAL_EXPONENTIAL;

int ProcessorParameters::packet_delivery_type = DELIVERY_WITHOUT_ACK;
int ProcessorParameters::sending_ack_type = SENDING_ACK_OPTIMAL;

//------- Synthetic parameters default

int SyntheticParameters::traffic_type = TRAFFIC_RANDOM;

int SyntheticParameters::neighbor_percentage = NEIGHBOR_PERCENTAGE;

int SyntheticParameters::regional_percentage = REGIONAL_PERCENTAGE;
int SyntheticParameters::regional_distance = REGIONAL_DISTANCE;

int SyntheticParameters::hotspot_type = HOTSPOT_CORNER; // hotspots are at corner
int SyntheticParameters::n_hotspots = 4;	// number of hotspots
HotSpotHashTable SyntheticParameters::hotspot_table = HotSpotHashTable();
int SyntheticParameters::hotspot_percentage = HOTSPOT_PERCENTAGE;

//---------- Dreamcloud default
string DCParameters::mapping = "";
string DCParameters::mappingFile = "";
string DCParameters::appXml = "";
string DCParameters::modeFile = "";
string DCParameters::outputFolder = "";
string DCParameters::scheduling = "";
unsigned int DCParameters::iterations = 1;
bool DCParameters::seqDep = false;
bool DCParameters::useMicroworkload = false;
unsigned int DCParameters::microworkloadWidth = 2;
unsigned int DCParameters::microworkloadHeight = 2;
bool DCParameters::generateWaveforms = false;
bool DCParameters::dontHandlePeriodic = false;
unsigned long int DCParameters::coresFrequencyInHz = 1E9;
int DCParameters::instructionsPerCycle = 1;

//---------- Embedded default
string EmbeddedParameters::app_folder = "src/embedded_app_graphs";
string EmbeddedParameters::app_filename = "80211arx.app";
int EmbeddedParameters::mapping_algorithm = MAPPING_RANDOM;
double EmbeddedParameters::max_req_bandwidth = 0;
EmbeddedAppHashTable EmbeddedParameters::app_info = EmbeddedAppHashTable();

//---------- Power Estimation
int CommonParameter::cmos_process = 65;

//---------- default buffer size is 16-flit
double PowerParameter_65nm::buffer_34b_rd_wr =
STM65_POWER_BUFFER_34b_16FLIT_RD_WR;
double PowerParameter_65nm::buffer_34b_rd_wr_clk =
STM65_POWER_BUFFER_34b_16FLIT_RD_WR_CLK;
double PowerParameter_65nm::buffer_34b_rd_wr_reg =
STM65_POWER_BUFFER_34b_16FLIT_RD_WR_REG;

double PowerParameter_65nm::buffer_34b_rd_only =
STM65_POWER_BUFFER_34b_16FLIT_RD_ONLY;
double PowerParameter_65nm::buffer_34b_rd_only_clk =
STM65_POWER_BUFFER_34b_16FLIT_RD_ONLY_CLK;
double PowerParameter_65nm::buffer_34b_rd_only_reg =
STM65_POWER_BUFFER_34b_16FLIT_RD_ONLY_REG;

double PowerParameter_65nm::buffer_34b_wr_only =
STM65_POWER_BUFFER_34b_16FLIT_WR_ONLY;
double PowerParameter_65nm::buffer_34b_wr_only_clk =
STM65_POWER_BUFFER_34b_16FLIT_WR_ONLY_CLK;
double PowerParameter_65nm::buffer_34b_wr_only_reg =
STM65_POWER_BUFFER_34b_16FLIT_WR_ONLY_REG;

double PowerParameter_65nm::buffer_34b_inactive =
STM65_POWER_BUFFER_34b_16FLIT_INACTIVE;
double PowerParameter_65nm::buffer_34b_inactive_clk =
STM65_POWER_BUFFER_34b_16FLIT_INACTIVE_CLK;
double PowerParameter_65nm::buffer_34b_inactive_reg =
STM65_POWER_BUFFER_34b_16FLIT_INACTIVE_REG;

double PowerParameter_65nm::buffer_34b_leak = STM65_POWER_BUFFER_34b_16FLIT_LEAK;

//---------- default routing algorithm is adaptive
double PowerParameter_65nm::routing_12b_head =
STM65_POWER_ROUTING_ADAPTIVE_12b_HEAD;
double PowerParameter_65nm::routing_12b_head_clk =
STM65_POWER_ROUTING_ADAPTIVE_12b_HEAD_CLK;
double PowerParameter_65nm::routing_12b_head_reg =
STM65_POWER_ROUTING_ADAPTIVE_12b_HEAD_REG;

double PowerParameter_65nm::routing_12b_head_stall =
STM65_POWER_ROUTING_ADAPTIVE_12b_HEAD_STALL;
double PowerParameter_65nm::routing_12b_head_stall_clk =
STM65_POWER_ROUTING_ADAPTIVE_12b_HEAD_STALL_CLK;
double PowerParameter_65nm::routing_12b_head_stall_reg =
STM65_POWER_ROUTING_ADAPTIVE_12b_HEAD_STALL_REG;

double PowerParameter_65nm::routing_12b_body =
STM65_POWER_ROUTING_ADAPTIVE_12b_BODY;
double PowerParameter_65nm::routing_12b_body_clk =
STM65_POWER_ROUTING_ADAPTIVE_12b_BODY_CLK;
double PowerParameter_65nm::routing_12b_body_reg =
STM65_POWER_ROUTING_ADAPTIVE_12b_BODY_REG;

double PowerParameter_65nm::routing_12b_inactive =
STM65_POWER_ROUTING_ADAPTIVE_12b_INACTIVE;
double PowerParameter_65nm::routing_12b_inactive_clk =
STM65_POWER_ROUTING_ADAPTIVE_12b_INACTIVE_CLK;
double PowerParameter_65nm::routing_12b_inactive_reg =
STM65_POWER_ROUTING_ADAPTIVE_12b_INACTIVE_REG;

double PowerParameter_65nm::routing_12b_leak =
STM65_POWER_ROUTING_ADAPTIVE_12b_LEAK;

double PowerParameter_65nm::arbiter_4inputs_active =
STM65_POWER_ARBITER_4INPUTS_ACTIVE;
double PowerParameter_65nm::arbiter_4inputs_active_clk =
STM65_POWER_ARBITER_4INPUTS_ACTIVE_CLK;
double PowerParameter_65nm::arbiter_4inputs_active_reg =
STM65_POWER_ARBITER_4INPUTS_ACTIVE_REG;

double PowerParameter_65nm::arbiter_4inputs_hold =
STM65_POWER_ARBITER_4INPUTS_HOLD;
double PowerParameter_65nm::arbiter_4inputs_hold_clk =
STM65_POWER_ARBITER_4INPUTS_HOLD_CLK;
double PowerParameter_65nm::arbiter_4inputs_hold_reg =
STM65_POWER_ARBITER_4INPUTS_HOLD_REG;

double PowerParameter_65nm::arbiter_4inputs_not_ready =
STM65_POWER_ARBITER_4INPUTS_NOT_READY;
double PowerParameter_65nm::arbiter_4inputs_not_ready_clk =
STM65_POWER_ARBITER_4INPUTS_NOT_READY_CLK;
double PowerParameter_65nm::arbiter_4inputs_not_ready_reg =
STM65_POWER_ARBITER_4INPUTS_NOT_READY_REG;

double PowerParameter_65nm::arbiter_4inputs_inactive =
STM65_POWER_ARBITER_4INPUTS_INACTIVE;
double PowerParameter_65nm::arbiter_4inputs_inactive_clk =
STM65_POWER_ARBITER_4INPUTS_INACTIVE_CLK;
double PowerParameter_65nm::arbiter_4inputs_inactive_reg =
STM65_POWER_ARBITER_4INPUTS_INACTIVE_REG;

double PowerParameter_65nm::arbiter_4inputs_leak =
STM65_POWER_ARBITER_4INPUTS_LEAK;

double PowerParameter_65nm::crossbar_34b_one_active =
STM65_POWER_CROSSBAR_34b_ONE_ACTIVE;
double PowerParameter_65nm::crossbar_34b_inactive =
STM65_POWER_CROSSBAR_34b_INACTIVE;
double PowerParameter_65nm::crossbar_34b_leak = STM65_POWER_CROSSBAR_34b_LEAK;

double PowerParameter_65nm::out_credit_counter_5b_one_on =
STM65_POWER_OUT_CREDIT_COUNTER_5b_ONE_ON;
double PowerParameter_65nm::out_credit_counter_5b_one_on_clk =
STM65_POWER_OUT_CREDIT_COUNTER_5b_ONE_ON_CLK;
double PowerParameter_65nm::out_credit_counter_5b_one_on_reg =
STM65_POWER_OUT_CREDIT_COUNTER_5b_ONE_ON_REG;

double PowerParameter_65nm::out_credit_counter_5b_one_changes =
STM65_POWER_OUT_CREDIT_COUNTER_5b_ONE_CHANGES;
double PowerParameter_65nm::out_credit_counter_5b_one_changes_clk =
STM65_POWER_OUT_CREDIT_COUNTER_5b_ONE_CHANGES_CLK;
double PowerParameter_65nm::out_credit_counter_5b_one_changes_reg =
STM65_POWER_OUT_CREDIT_COUNTER_5b_ONE_CHANGES_REG;

double PowerParameter_65nm::out_credit_counter_5b_both_change_diff =
STM65_POWER_OUT_CREDIT_COUNTER_5b_BOTH_CHANGE_DIFF;
double PowerParameter_65nm::out_credit_counter_5b_both_change_diff_clk =
STM65_POWER_OUT_CREDIT_COUNTER_5b_BOTH_CHANGE_DIFF_CLK;
double PowerParameter_65nm::out_credit_counter_5b_both_change_diff_reg =
STM65_POWER_OUT_CREDIT_COUNTER_5b_BOTH_CHANGE_DIFF_REG;

double PowerParameter_65nm::out_credit_counter_5b_inactive =
STM65_POWER_OUT_CREDIT_COUNTER_5b_INACTIVE;
double PowerParameter_65nm::out_credit_counter_5b_inactive_clk =
STM65_POWER_OUT_CREDIT_COUNTER_5b_INACTIVE_CLK;
double PowerParameter_65nm::out_credit_counter_5b_inactive_reg =
STM65_POWER_OUT_CREDIT_COUNTER_5b_INACTIVE_REG;

double PowerParameter_65nm::out_credit_counter_5b_leak =
STM65_POWER_OUT_CREDIT_COUNTER_5b_LEAK;

double PowerParameter_65nm::link_1000um_34b_active =
PTM65_POWER_LINK_1000um_34b_ACTIVE;
double PowerParameter_65nm::link_1000um_34b_inactive =
PTM65_POWER_LINK_1000um_34b_INACTIVE;
double PowerParameter_65nm::link_1000um_34b_leak =
PTM65_POWER_LINK_1000um_34b_LEAK;

double PowerParameter_65nm::pipeline_34b_active =
STM65_POWER_PIPELINE_34b_ACTIVE;
double PowerParameter_65nm::pipeline_34b_active_clk =
STM65_POWER_PIPELINE_34b_ACTIVE_CLK;
double PowerParameter_65nm::pipeline_34b_active_reg =
STM65_POWER_PIPELINE_34b_ACTIVE_REG;

double PowerParameter_65nm::pipeline_34b_inactive =
STM65_POWER_PIPELINE_34b_INACTIVE;
double PowerParameter_65nm::pipeline_34b_inactive_clk =
STM65_POWER_PIPELINE_34b_INACTIVE_CLK;
double PowerParameter_65nm::pipeline_34b_inactive_reg =
STM65_POWER_PIPELINE_34b_INACTIVE_REG;

double PowerParameter_65nm::pipeline_34b_leak = STM65_POWER_PIPELINE_34b_LEAK;

double PowerParameter_65nm::pipeline_5b_active = STM65_POWER_PIPELINE_5b_ACTIVE;
double PowerParameter_65nm::pipeline_5b_active_clk =
STM65_POWER_PIPELINE_5b_ACTIVE_CLK;
double PowerParameter_65nm::pipeline_5b_active_reg =
STM65_POWER_PIPELINE_5b_ACTIVE_REG;

double PowerParameter_65nm::pipeline_5b_inactive =
STM65_POWER_PIPELINE_5b_INACTIVE;
double PowerParameter_65nm::pipeline_5b_inactive_clk =
STM65_POWER_PIPELINE_5b_INACTIVE_CLK;
double PowerParameter_65nm::pipeline_5b_inactive_reg =
STM65_POWER_PIPELINE_5b_INACTIVE_REG;

double PowerParameter_65nm::pipeline_5b_leak = STM65_POWER_PIPELINE_5b_LEAK;

double PowerParameter_65nm::global_clock_tree = 9050;	// 4x4 = 16 routers

//----------- default voltage and freq
double CommonParameter::input_voltage = 1.0;	// voltage (V)

int CommonParameter::clk_freq_mode = CLK_FREQ_FIXED;	// fixed or maximum
double CommonParameter::input_clk_freq = 1000;	// clk rate (MHz)
double CommonParameter::operating_clk_freq = 1000;

double TimingParameter_65nm::setup = STANDARD_65nm_DELAY_SETUP_1_3V;
double TimingParameter_65nm::clk_q = STANDARD_65nm_DELAY_CLK_Q_1_3V;

double TimingParameter_65nm::BW = STANDARD_65nm_DELAY_BW_16FLITS_1_3V; // default bsize = 16
double TimingParameter_65nm::RC = STANDARD_65nm_DELAY_RC_ADAPTIVE_1_3V; // default adaptive routing
double TimingParameter_65nm::SA = STANDARD_65nm_DELAY_SA_1_3V;
double TimingParameter_65nm::ST = STANDARD_65nm_DELAY_ST_1_3V;
double TimingParameter_65nm::LT = STANDARD_65nm_DELAY_LT_1000um_1_3V;

//================= prototypes
void global_clock_tree_power_cal();	// including PLL, clk buffers, wires

void timing_power_regulation();	// regulate delay and power of each component according to voltage and pipeline type

void power_energy_cal(Platform *platform);// calculate and display the overall power, energy

/**
 * Return a human readable string of the given frequency.
 */
string getFrequencyString(unsigned long int freqInHertz) {
	if (freqInHertz < 1E3) {
		return std::to_string(freqInHertz) + " Hz";
	} else if (freqInHertz >= 1E3 && freqInHertz < 1E6) {
		return std::to_string((unsigned long int) (freqInHertz / 1E3)) + " KHz";
	} else if (freqInHertz >= 1E6 && freqInHertz < 1E9) {
		return std::to_string((unsigned long int) (freqInHertz / 1E6)) + " MHz";
	} else if (freqInHertz >= 1E9) {
		return std::to_string((unsigned long int) (freqInHertz / 1E9)) + " GHz";
	} else {
		cerr << "Invalid Frequency in hertz" << freqInHertz << endl;
		exit(-1);
	}
}

//================= MAIN ===========================
int sc_main(int argc, char *argv[]) {

	time_t begin_run = time(0);
	clock_t begin_system_clock = clock();
	//tm *ptm = localtime(&begin_run);

	//---------- Command line parse
	command_line_parser(argc, argv);

	// set random seed before mapping tasks to the network
	srand(CommonParameter::random_seed);

	if (CommonParameter::platform_type == PLATFORM_EMBEDDED) {
		string app_filename = "";
		app_filename = app_filename + EmbeddedParameters::app_folder + "/"
				+ EmbeddedParameters::app_filename;
		EmbeddedAppInfo *embedded_app_info = new EmbeddedAppInfo(
				app_filename.data());

		CommonParameter::dim_x = embedded_app_info->dim_x;
		CommonParameter::dim_y = embedded_app_info->dim_y;

		EmbeddedParameters::max_req_bandwidth =
				embedded_app_info->max_req_bandwidth;
		EmbeddedParameters::app_info = embedded_app_info->app_info;

		delete embedded_app_info;
	}

	if (SyntheticParameters::traffic_type == TRAFFIC_HOTSPOT) {
		HotSpotHashTable hotspot_table;
		if (SyntheticParameters::hotspot_type == HOTSPOT_CENTRAL) {	// central hotspots
			switch (SyntheticParameters::n_hotspots) {
			case (1): {
				TileLocation hotspot = TileLocation(
						(int) floor(CommonParameter::dim_x / 2),
						(int) floor(CommonParameter::dim_y / 2));
				hotspot_table.insert(HotSpotHashTable::value_type(0, hotspot));
			}
				break;
			case (2): {
				TileLocation hotspot0 = TileLocation(
						(int) ceil(CommonParameter::dim_x / 2),
						(int) ceil(CommonParameter::dim_y / 2));
				hotspot_table.insert(HotSpotHashTable::value_type(0, hotspot0));

				TileLocation hotspot1 = TileLocation(
						(int) ceil(CommonParameter::dim_x / 2) - 1,
						(int) ceil(CommonParameter::dim_y / 2) - 1);
				hotspot_table.insert(HotSpotHashTable::value_type(1, hotspot1));
			}
				break;
			case (4): {
				TileLocation hotspot0 = TileLocation(
						(int) ceil(CommonParameter::dim_x / 2),
						(int) ceil(CommonParameter::dim_y / 2));
				hotspot_table.insert(HotSpotHashTable::value_type(0, hotspot0));

				TileLocation hotspot1 = TileLocation(
						(int) ceil(CommonParameter::dim_x / 2) - 1,
						(int) ceil(CommonParameter::dim_y / 2));
				hotspot_table.insert(HotSpotHashTable::value_type(1, hotspot1));

				TileLocation hotspot2 = TileLocation(
						(int) ceil(CommonParameter::dim_x / 2),
						(int) ceil(CommonParameter::dim_y / 2) - 1);
				hotspot_table.insert(HotSpotHashTable::value_type(2, hotspot2));

				TileLocation hotspot3 = TileLocation(
						(int) ceil(CommonParameter::dim_x / 2) - 1,
						(int) ceil(CommonParameter::dim_y / 2) - 1);
				hotspot_table.insert(HotSpotHashTable::value_type(3, hotspot3));
			}
				break;
			default: {
				cout << "******ERROR: number of hotspots is not supported!"
						<< endl;
				exit(0);
			}
			}
		} else {	// corner hotspots
			switch (SyntheticParameters::n_hotspots) {
			case (1): {
				TileLocation hotspot = TileLocation(0, 0);
				hotspot_table.insert(HotSpotHashTable::value_type(0, hotspot));
			}
				break;
			case (2): {
				TileLocation hotspot0 = TileLocation(0, 0);
				hotspot_table.insert(HotSpotHashTable::value_type(0, hotspot0));

				TileLocation hotspot1 = TileLocation(CommonParameter::dim_x - 1,
						0);
				hotspot_table.insert(HotSpotHashTable::value_type(1, hotspot1));
			}
				break;
			case (4): {
				TileLocation hotspot0 = TileLocation(0, 0);
				hotspot_table.insert(HotSpotHashTable::value_type(0, hotspot0));

				TileLocation hotspot1 = TileLocation(CommonParameter::dim_x - 1,
						0);
				hotspot_table.insert(HotSpotHashTable::value_type(1, hotspot1));

				TileLocation hotspot2 = TileLocation(0,
						CommonParameter::dim_y - 1);
				hotspot_table.insert(HotSpotHashTable::value_type(2, hotspot2));

				TileLocation hotspot3 = TileLocation(CommonParameter::dim_x - 1,
						CommonParameter::dim_y - 1);
				hotspot_table.insert(HotSpotHashTable::value_type(3, hotspot3));
			}
				break;
			default:
				cout << "******ERROR: number of hotspots is not supported!"
						<< endl;
				exit(0);
				break;
			}
		}

		SyntheticParameters::hotspot_table = hotspot_table;
	}

	//================================================
	//--------- get component power
	switch (RouterParameter::buffer_size) {
	case (1):
		PowerParameter_65nm::buffer_34b_rd_wr =
		STM65_POWER_BUFFER_34b_1FLIT_RD_WR;
		PowerParameter_65nm::buffer_34b_rd_wr_clk =
		STM65_POWER_BUFFER_34b_1FLIT_RD_WR_CLK;
		PowerParameter_65nm::buffer_34b_rd_wr_reg =
		STM65_POWER_BUFFER_34b_1FLIT_RD_WR_REG;

		PowerParameter_65nm::buffer_34b_rd_only =
		STM65_POWER_BUFFER_34b_1FLIT_RD_ONLY;
		PowerParameter_65nm::buffer_34b_rd_only_clk =
		STM65_POWER_BUFFER_34b_1FLIT_RD_ONLY_CLK;
		PowerParameter_65nm::buffer_34b_rd_only_reg =
		STM65_POWER_BUFFER_34b_1FLIT_RD_ONLY_REG;

		PowerParameter_65nm::buffer_34b_wr_only =
		STM65_POWER_BUFFER_34b_1FLIT_WR_ONLY;
		PowerParameter_65nm::buffer_34b_wr_only_clk =
		STM65_POWER_BUFFER_34b_1FLIT_WR_ONLY_CLK;
		PowerParameter_65nm::buffer_34b_wr_only_reg =
		STM65_POWER_BUFFER_34b_1FLIT_WR_ONLY_REG;

		PowerParameter_65nm::buffer_34b_inactive =
		STM65_POWER_BUFFER_34b_1FLIT_INACTIVE;
		PowerParameter_65nm::buffer_34b_inactive_clk =
		STM65_POWER_BUFFER_34b_1FLIT_INACTIVE_CLK;
		PowerParameter_65nm::buffer_34b_inactive_reg =
		STM65_POWER_BUFFER_34b_1FLIT_INACTIVE_REG;
		PowerParameter_65nm::buffer_34b_leak =
		STM65_POWER_BUFFER_34b_1FLIT_LEAK;

		break;

	case (2):
		PowerParameter_65nm::buffer_34b_rd_wr =
		STM65_POWER_BUFFER_34b_2FLIT_RD_WR;
		PowerParameter_65nm::buffer_34b_rd_wr_clk =
		STM65_POWER_BUFFER_34b_2FLIT_RD_WR_CLK;
		PowerParameter_65nm::buffer_34b_rd_wr_reg =
		STM65_POWER_BUFFER_34b_2FLIT_RD_WR_REG;

		PowerParameter_65nm::buffer_34b_rd_only =
		STM65_POWER_BUFFER_34b_2FLIT_RD_ONLY;
		PowerParameter_65nm::buffer_34b_rd_only_clk =
		STM65_POWER_BUFFER_34b_2FLIT_RD_ONLY_CLK;
		PowerParameter_65nm::buffer_34b_rd_only_reg =
		STM65_POWER_BUFFER_34b_2FLIT_RD_ONLY_REG;

		PowerParameter_65nm::buffer_34b_wr_only =
		STM65_POWER_BUFFER_34b_2FLIT_WR_ONLY;
		PowerParameter_65nm::buffer_34b_wr_only_clk =
		STM65_POWER_BUFFER_34b_2FLIT_WR_ONLY_CLK;
		PowerParameter_65nm::buffer_34b_wr_only_reg =
		STM65_POWER_BUFFER_34b_2FLIT_WR_ONLY_REG;

		PowerParameter_65nm::buffer_34b_inactive =
		STM65_POWER_BUFFER_34b_2FLIT_INACTIVE;
		PowerParameter_65nm::buffer_34b_inactive_clk =
		STM65_POWER_BUFFER_34b_2FLIT_INACTIVE_CLK;
		PowerParameter_65nm::buffer_34b_inactive_reg =
		STM65_POWER_BUFFER_34b_2FLIT_INACTIVE_REG;
		PowerParameter_65nm::buffer_34b_leak =
		STM65_POWER_BUFFER_34b_2FLIT_LEAK;

		break;
	case (4):
		PowerParameter_65nm::buffer_34b_rd_wr =
		STM65_POWER_BUFFER_34b_4FLIT_RD_WR;
		PowerParameter_65nm::buffer_34b_rd_wr_clk =
		STM65_POWER_BUFFER_34b_4FLIT_RD_WR_CLK;
		PowerParameter_65nm::buffer_34b_rd_wr_reg =
		STM65_POWER_BUFFER_34b_4FLIT_RD_WR_REG;

		PowerParameter_65nm::buffer_34b_rd_only =
		STM65_POWER_BUFFER_34b_4FLIT_RD_ONLY;
		PowerParameter_65nm::buffer_34b_rd_only_clk =
		STM65_POWER_BUFFER_34b_4FLIT_RD_ONLY_CLK;
		PowerParameter_65nm::buffer_34b_rd_only_reg =
		STM65_POWER_BUFFER_34b_4FLIT_RD_ONLY_REG;

		PowerParameter_65nm::buffer_34b_wr_only =
		STM65_POWER_BUFFER_34b_4FLIT_WR_ONLY;
		PowerParameter_65nm::buffer_34b_wr_only_clk =
		STM65_POWER_BUFFER_34b_4FLIT_WR_ONLY_CLK;
		PowerParameter_65nm::buffer_34b_wr_only_reg =
		STM65_POWER_BUFFER_34b_4FLIT_WR_ONLY_REG;

		PowerParameter_65nm::buffer_34b_inactive =
		STM65_POWER_BUFFER_34b_4FLIT_INACTIVE;
		PowerParameter_65nm::buffer_34b_inactive_clk =
		STM65_POWER_BUFFER_34b_4FLIT_INACTIVE_CLK;
		PowerParameter_65nm::buffer_34b_inactive_reg =
		STM65_POWER_BUFFER_34b_4FLIT_INACTIVE_REG;
		PowerParameter_65nm::buffer_34b_leak =
		STM65_POWER_BUFFER_34b_4FLIT_LEAK;

		break;
	case (8):
		PowerParameter_65nm::buffer_34b_rd_wr =
		STM65_POWER_BUFFER_34b_8FLIT_RD_WR;
		PowerParameter_65nm::buffer_34b_rd_wr_clk =
		STM65_POWER_BUFFER_34b_8FLIT_RD_WR_CLK;
		PowerParameter_65nm::buffer_34b_rd_wr_reg =
		STM65_POWER_BUFFER_34b_8FLIT_RD_WR_REG;

		PowerParameter_65nm::buffer_34b_rd_only =
		STM65_POWER_BUFFER_34b_8FLIT_RD_ONLY;
		PowerParameter_65nm::buffer_34b_rd_only_clk =
		STM65_POWER_BUFFER_34b_8FLIT_RD_ONLY_CLK;
		PowerParameter_65nm::buffer_34b_rd_only_reg =
		STM65_POWER_BUFFER_34b_8FLIT_RD_ONLY_REG;

		PowerParameter_65nm::buffer_34b_wr_only =
		STM65_POWER_BUFFER_34b_8FLIT_WR_ONLY;
		PowerParameter_65nm::buffer_34b_wr_only_clk =
		STM65_POWER_BUFFER_34b_8FLIT_WR_ONLY_CLK;
		PowerParameter_65nm::buffer_34b_wr_only_reg =
		STM65_POWER_BUFFER_34b_8FLIT_WR_ONLY_REG;

		PowerParameter_65nm::buffer_34b_inactive =
		STM65_POWER_BUFFER_34b_8FLIT_INACTIVE;
		PowerParameter_65nm::buffer_34b_inactive_clk =
		STM65_POWER_BUFFER_34b_8FLIT_INACTIVE_CLK;
		PowerParameter_65nm::buffer_34b_inactive_reg =
		STM65_POWER_BUFFER_34b_8FLIT_INACTIVE_REG;
		PowerParameter_65nm::buffer_34b_leak =
		STM65_POWER_BUFFER_34b_8FLIT_LEAK;

		break;
	case (16):
		PowerParameter_65nm::buffer_34b_rd_wr =
		STM65_POWER_BUFFER_34b_16FLIT_RD_WR;
		PowerParameter_65nm::buffer_34b_rd_wr_clk =
		STM65_POWER_BUFFER_34b_16FLIT_RD_WR_CLK;
		PowerParameter_65nm::buffer_34b_rd_wr_reg =
		STM65_POWER_BUFFER_34b_16FLIT_RD_WR_REG;

		PowerParameter_65nm::buffer_34b_rd_only =
		STM65_POWER_BUFFER_34b_16FLIT_RD_ONLY;
		PowerParameter_65nm::buffer_34b_rd_only_clk =
		STM65_POWER_BUFFER_34b_16FLIT_RD_ONLY_CLK;
		PowerParameter_65nm::buffer_34b_rd_only_reg =
		STM65_POWER_BUFFER_34b_16FLIT_RD_ONLY_REG;

		PowerParameter_65nm::buffer_34b_wr_only =
		STM65_POWER_BUFFER_34b_16FLIT_WR_ONLY;
		PowerParameter_65nm::buffer_34b_wr_only_clk =
		STM65_POWER_BUFFER_34b_16FLIT_WR_ONLY_CLK;
		PowerParameter_65nm::buffer_34b_wr_only_reg =
		STM65_POWER_BUFFER_34b_16FLIT_WR_ONLY_REG;

		PowerParameter_65nm::buffer_34b_inactive =
		STM65_POWER_BUFFER_34b_16FLIT_INACTIVE;
		PowerParameter_65nm::buffer_34b_inactive_clk =
		STM65_POWER_BUFFER_34b_16FLIT_INACTIVE_CLK;
		PowerParameter_65nm::buffer_34b_inactive_reg =
		STM65_POWER_BUFFER_34b_16FLIT_INACTIVE_REG;
		PowerParameter_65nm::buffer_34b_leak =
		STM65_POWER_BUFFER_34b_16FLIT_LEAK;

		break;
	default:
		cout << "ERROR: buffer size is not supported!" << endl;
		exit(0);
	}

	if (RouterParameter::routing_type == ROUTING_XY) {
		PowerParameter_65nm::routing_12b_head = STM65_POWER_ROUTING_XY_12b_HEAD;
		PowerParameter_65nm::routing_12b_head_clk =
		STM65_POWER_ROUTING_XY_12b_HEAD_CLK;
		PowerParameter_65nm::routing_12b_head_reg =
		STM65_POWER_ROUTING_XY_12b_HEAD_REG;

		PowerParameter_65nm::routing_12b_head_stall =
		STM65_POWER_ROUTING_XY_12b_HEAD_STALL;
		PowerParameter_65nm::routing_12b_head_stall_clk =
		STM65_POWER_ROUTING_XY_12b_HEAD_STALL_CLK;
		PowerParameter_65nm::routing_12b_head_stall_reg =
		STM65_POWER_ROUTING_XY_12b_HEAD_STALL_REG;

		PowerParameter_65nm::routing_12b_body = STM65_POWER_ROUTING_XY_12b_BODY;
		PowerParameter_65nm::routing_12b_body_clk =
		STM65_POWER_ROUTING_XY_12b_BODY_CLK;
		PowerParameter_65nm::routing_12b_body_reg =
		STM65_POWER_ROUTING_XY_12b_BODY_REG;

		PowerParameter_65nm::routing_12b_inactive =
		STM65_POWER_ROUTING_XY_12b_INACTIVE;
		PowerParameter_65nm::routing_12b_inactive_clk =
		STM65_POWER_ROUTING_XY_12b_INACTIVE_CLK;
		PowerParameter_65nm::routing_12b_inactive_reg =
		STM65_POWER_ROUTING_XY_12b_INACTIVE_REG;

		PowerParameter_65nm::routing_12b_leak = STM65_POWER_ROUTING_XY_12b_LEAK;
	} else {
		PowerParameter_65nm::routing_12b_head =
		STM65_POWER_ROUTING_ADAPTIVE_12b_HEAD;
		PowerParameter_65nm::routing_12b_head_clk =
		STM65_POWER_ROUTING_ADAPTIVE_12b_HEAD_CLK;
		PowerParameter_65nm::routing_12b_head_reg =
		STM65_POWER_ROUTING_ADAPTIVE_12b_HEAD_REG;

		PowerParameter_65nm::routing_12b_head_stall =
		STM65_POWER_ROUTING_ADAPTIVE_12b_HEAD_STALL;
		PowerParameter_65nm::routing_12b_head_stall_clk =
		STM65_POWER_ROUTING_ADAPTIVE_12b_HEAD_STALL_CLK;
		PowerParameter_65nm::routing_12b_head_stall_reg =
		STM65_POWER_ROUTING_ADAPTIVE_12b_HEAD_STALL_REG;

		PowerParameter_65nm::routing_12b_body =
		STM65_POWER_ROUTING_ADAPTIVE_12b_BODY;
		PowerParameter_65nm::routing_12b_body_clk =
		STM65_POWER_ROUTING_ADAPTIVE_12b_BODY_CLK;
		PowerParameter_65nm::routing_12b_body_reg =
		STM65_POWER_ROUTING_ADAPTIVE_12b_BODY_REG;

		PowerParameter_65nm::routing_12b_inactive =
		STM65_POWER_ROUTING_ADAPTIVE_12b_INACTIVE;
		PowerParameter_65nm::routing_12b_inactive_clk =
		STM65_POWER_ROUTING_ADAPTIVE_12b_INACTIVE_CLK;
		PowerParameter_65nm::routing_12b_inactive_reg =
		STM65_POWER_ROUTING_ADAPTIVE_12b_INACTIVE_REG;

		PowerParameter_65nm::routing_12b_leak =
		STM65_POWER_ROUTING_ADAPTIVE_12b_LEAK;
	}

	//----------- get timing info
	switch (RouterParameter::buffer_size) {
	case (1):
		TimingParameter_65nm::BW = 0;
		break;
	case (2):
		TimingParameter_65nm::BW = STANDARD_65nm_DELAY_BW_2FLITS_1_3V;
		break;
	case (4):
		TimingParameter_65nm::BW = STANDARD_65nm_DELAY_BW_4FLITS_1_3V;
		break;
	case (8):
		TimingParameter_65nm::BW = STANDARD_65nm_DELAY_BW_8FLITS_1_3V;
		break;
	case (16):
		TimingParameter_65nm::BW = STANDARD_65nm_DELAY_BW_16FLITS_1_3V;
		break;
	default:
		;
	}

	if (RouterParameter::routing_type == ROUTING_XY) {
		TimingParameter_65nm::RC = STANDARD_65nm_DELAY_RC_XY_1_3V;
	} else {
		TimingParameter_65nm::RC = STANDARD_65nm_DELAY_RC_ADAPTIVE_1_3V;
	}

	// assuming link delay is proportional to link length (using repeaters)
	double link_ratio = RouterParameter::link_length / 1000;
	TimingParameter_65nm::LT = TimingParameter_65nm::LT * link_ratio;

	//----------- get router pipelining info
	switch (RouterParameter::pipeline_type) {
	case PIPELINE_TYPE_1:
		RouterParameter::n_pipeline_stages = 1;
		break;
	case PIPELINE_TYPE_2_1:
		RouterParameter::n_pipeline_stages = 2;
		break;
	case PIPELINE_TYPE_2_2:
		RouterParameter::n_pipeline_stages = 2;
		break;
	case PIPELINE_TYPE_2_3:
		RouterParameter::n_pipeline_stages = 2;
		break;
	case PIPELINE_TYPE_3_1:
		RouterParameter::n_pipeline_stages = 3;
		break;
	case PIPELINE_TYPE_3_2:
		RouterParameter::n_pipeline_stages = 3;
		break;
	case PIPELINE_TYPE_3_3:
		RouterParameter::n_pipeline_stages = 3;
		break;
	case PIPELINE_TYPE_4:
		RouterParameter::n_pipeline_stages = 4;
		break;
	default:
		cout << "ERROR: pipeline type is not supported!" << endl;
		exit(0);
	}

	//-------- global clock tree power (don't count subtrees inside routers)
	global_clock_tree_power_cal();

	//------- power regulation depending on pipeline type and voltage
	timing_power_regulation();

	sc_clock clk("clk", 1, SC_NS);
	sc_signal<bool> reset;

	// Print simulation settings

	if (CommonParameter::platform_type != PLATFORM_DC) {
		cout << " ##Configuration Information##" << endl;
		cout << "Random seed = " << CommonParameter::random_seed << endl;
		cout << "Network size = " << CommonParameter::dim_x << " x "
				<< CommonParameter::dim_y << " (nodes)" << endl;
		string platform_type;
		switch (CommonParameter::platform_type) {
		case PLATFORM_SYNTHETIC:
			platform_type = "SYNTHETIC";
			break;
		case PLATFORM_EMBEDDED:
			platform_type = "EMBEDDED";
			break;
		default:
			;
		}
		cout << "Platform type: " << platform_type << endl;

		show_processor_settings();

		if (CommonParameter::platform_type == PLATFORM_SYNTHETIC) {
			show_synthetic_settings();
		} else if (CommonParameter::platform_type == PLATFORM_EMBEDDED) {
			show_embedded_settings();
		}

		show_router_settings();

		cout << "Warm_up time = " << CommonParameter::warmup_time
				<< " cycles ..." << endl << endl;
	} else {
		cout << endl;
		cout
				<< "***************************************************************"
				<< endl;
		cout
				<< "***** Summary of platform (cores + NoC) simulation results ****"
				<< endl;
		cout
				<< "***************************************************************"
				<< endl;
		cout << endl << endl;
		cout << " ##Configuration information##" << endl << endl;
		if (DCParameters::appXml.empty()) {
			cout << "    Input mode switchig file           : "
					<< DCParameters::modeFile << endl;
		} else {
			cout << "    Input application                  : "
					<< DCParameters::appXml << endl;
		}
		cout << "    Results output folder              : "
				<< DCParameters::outputFolder << endl;
		cout << "    Number of cores   in platform      : "
				<< CommonParameter::dim_x * CommonParameter::dim_y << endl;
		cout << "    Number of rows    in platform      : "
				<< CommonParameter::dim_x << endl;
		cout << "    Number of columns in platform      : "
				<< CommonParameter::dim_y << endl;
		cout << "    Cores operating frequency          : "
				<< getFrequencyString(DCParameters::coresFrequencyInHz) << endl;
		cout << "    Mapping heuristic                  : "
				<< DCParameters::mapping;
		if (DCParameters::mapping == "Static"
				|| DCParameters::mapping == "StaticSM") {
			cout << " " << DCParameters::mappingFile;
		}
		cout << endl;
		cout << "    Runnables scheduling strategy      : "
				<< DCParameters::scheduling << endl;
		cout << "    Syntactic dependency consideration : "
				<< (DCParameters::seqDep ? "true" : "false") << endl;
		cout << "    Waveform  generation               : "
				<< (DCParameters::generateWaveforms ? "true" : "false") << endl;
		cout << endl << endl;

	}

	// Create a NoC-based platform
	Platform *noc = NULL;
	DcPlatform *dcNoc = NULL;
	if (CommonParameter::platform_type != PLATFORM_DC) {
		Platform *noc = new Platform("Platform");
		noc->clk(clk);
		noc->reset(reset);
		// not allow to input or output at network boundary
		for (int x = 0; x < CommonParameter::dim_x; x++) {
			noc->N_valid_in[x][0].write(0);	// no input from the North boundary
			for (int k = 0; k < MAX_N_VCS; k++) {
				noc->N_out_vc_buffer_rd[x][0][k].write(0);// not allow any output to the North boundary
			}

			noc->N_valid_out[x][CommonParameter::dim_y].write(0);// no input from the South boundary
			for (int k = 0; k < MAX_N_VCS; k++) {
				noc->N_in_vc_buffer_rd[x][CommonParameter::dim_y][k].write(0);// not allow any output to the South boundary
			}
		}

		for (int y = 0; y < CommonParameter::dim_y; y++) {
			noc->W_valid_in[0][y].write(0);	// no input from the West boundary
			for (int k = 0; k < MAX_N_VCS; k++) {
				noc->W_out_vc_buffer_rd[0][y][k].write(0);// not allow any output to the West boundary
			}

			noc->W_valid_out[CommonParameter::dim_x][y].write(0);// no input from the East boundary
			for (int k = 0; k < MAX_N_VCS; k++) {
				noc->W_in_vc_buffer_rd[CommonParameter::dim_x][y][k].write(0);// not allow any output to the East boundary
			}
		}
	} else {
		dcNoc = new DcPlatform("Platform");
		dcNoc->clk(clk);
		dcNoc->reset(reset);
		// not allow to input or output at network boundary
		for (int x = 0; x < CommonParameter::dim_x; x++) {
			dcNoc->N_valid_in[x][0].write(0);// no input from the North boundary
			for (int k = 0; k < MAX_N_VCS; k++) {
				dcNoc->N_out_vc_buffer_rd[x][0][k].write(0);// not allow any output to the North boundary
			}

			dcNoc->N_valid_out[x][CommonParameter::dim_y].write(0);	// no input from the South boundary
			for (int k = 0; k < MAX_N_VCS; k++) {
				dcNoc->N_in_vc_buffer_rd[x][CommonParameter::dim_y][k].write(0);// not allow any output to the South boundary
			}
		}

		for (int y = 0; y < CommonParameter::dim_y; y++) {
			dcNoc->W_valid_in[0][y].write(0);// no input from the West boundary
			for (int k = 0; k < MAX_N_VCS; k++) {
				dcNoc->W_out_vc_buffer_rd[0][y][k].write(0);// not allow any output to the West boundary
			}

			dcNoc->W_valid_out[CommonParameter::dim_x][y].write(0);	// no input from the East boundary
			for (int k = 0; k < MAX_N_VCS; k++) {
				dcNoc->W_in_vc_buffer_rd[CommonParameter::dim_x][y][k].write(0);// not allow any output to the East boundary
			}
		}
	}

#ifdef MEM_PROF
	// Print memory profiling information
	std::cout << "there are " << sc_object::instance_count << " sc_objects " << " in memory" << std::endl;
	std::size_t total = 0;
	MemoryProfiled<DcPlatform>::print(total);
	MemoryProfiled<DcProc>::print(total);
	switch (RouterParameter::n_VCs) {
	case (1):
		MemoryProfiled<VCRTRouter<1>>::print(total);
		MemoryProfiled<VCRTCrossbar<1>>::print(total);
		MemoryProfiled<VCRTAllocator<1>>::print(total);
		MemoryProfiled<SWRTAllocator<1>>::print(total);
		MemoryProfiled<OutVCStateUpdateRT<1>>::print(total);
		MemoryProfiled<SAStage1RT<1>>::print(total);
		MemoryProfiled<SAStage2RT<1>>::print(total);
		MemoryProfiled<VCAStage1OutputVCChooserRT<1>>::print(total);
		MemoryProfiled<VCAStage2OutputVCArbitrerRT<1>>::print(total);
		break;
	case (2):
		MemoryProfiled<VCRTRouter<2>>::print(total);
		MemoryProfiled<VCRTCrossbar<2>>::print(total);
		MemoryProfiled<VCRTAllocator<2>>::print(total);
		MemoryProfiled<SWRTAllocator<2>>::print(total);
		MemoryProfiled<OutVCStateUpdateRT<2>>::print(total);
		MemoryProfiled<SAStage1RT<2>>::print(total);
		MemoryProfiled<SAStage2RT<2>>::print(total);
		MemoryProfiled<VCAStage1OutputVCChooserRT<2>>::print(total);
		MemoryProfiled<VCAStage2OutputVCArbitrerRT<2>>::print(total);
		break;
	case (4):
		MemoryProfiled<VCRTRouter<4>>::print(total);
		MemoryProfiled<VCRTCrossbar<4>>::print(total);
		MemoryProfiled<VCRTAllocator<4>>::print(total);
		MemoryProfiled<SWRTAllocator<4>>::print(total);
		MemoryProfiled<OutVCStateUpdateRT<4>>::print(total);
		MemoryProfiled<SAStage1RT<4>>::print(total);
		MemoryProfiled<SAStage2RT<4>>::print(total);
		MemoryProfiled<VCAStage1OutputVCChooserRT<4>>::print(total);
	    MemoryProfiled<VCAStage2OutputVCArbitrerRT<4>>::print(total);
		break;
	case (8):
		MemoryProfiled<VCRTRouter<8>>::print(total);
		MemoryProfiled<VCRTCrossbar<8>>::print(total);
		MemoryProfiled<VCRTAllocator<8>>::print(total);
		MemoryProfiled<SWRTAllocator<8>>::print(total);
		MemoryProfiled<OutVCStateUpdateRT<8>>::print(total);
		MemoryProfiled<SAStage1RT<8>>::print(total);
		MemoryProfiled<SAStage2RT<8>>::print(total);
		MemoryProfiled<VCAStage1OutputVCChooserRT<8>>::print(total);
		MemoryProfiled<VCAStage2OutputVCArbitrerRT<8>>::print(total);
		break;
	}
	MemoryProfiled<VCRTBuffer>::print(total);
	MemoryProfiled<VCValidInDemuxRT>::print(total);
	MemoryProfiled<InVCStateUpdateRT>::print(total);
	MemoryProfiled<RouteCompVcRT>::print(total);
	std::cout << "Total memory is " << (total / 1000.0) << " Kb" << std::endl << std::endl;
#endif

	//################## Running Simulation
	if (CommonParameter::platform_type != PLATFORM_DC) {

		reset.write(1);
		sc_start(5, SC_NS);		// reset 5 sycles
		reset.write(0);

		// Run simulation
		sc_start(1, SC_NS);
		if (CommonParameter::sim_mode == SIM_MODE_CYCLE) {
			cout << "Running simulation for "
					<< CommonParameter::simulation_time << " cycles ..."
					<< endl;
			sc_start(CommonParameter::simulation_time, SC_NS);
		} else {
			cout << "Running simulation until receiving at least "
					<< CommonParameter::max_n_rx_packets
					<< " packets after warming up..." << endl;
			sc_start();		// run until receiving enough required packets
		}

	} else {
		reset.write(1);
		sc_start(0, SC_NS);		// reset 1 cycle
		reset.write(0);

		// Create trace file
		sc_trace_file* wf = NULL;
		if (DCParameters::generateWaveforms) {
			wf = sc_create_vcd_trace_file(
					(DCParameters::outputFolder + "/"
							+ CommonParameter::vcd_filename).c_str());
			wf->set_time_unit(1, SC_PS);
			sc_trace(wf, clk, "clk");
			WormholePipeline *wh_pipeline[MAX_DIM][MAX_DIM];

			for (int x = 0; x < CommonParameter::dim_x; x++) {
				for (int y = 0; y < CommonParameter::dim_y; y++) {
					wh_pipeline[x][y] =
							(WormholePipeline*) dcNoc->tile[x][y]->router;

					// Trace routers input/output signals
					for (int i = 0; i < ROUTER_PORTS; i++) {
						string direction;
						switch (i) {
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
							;
						}

						string label;
						if (i == LOCAL) {
							label = "(" + int_to_str(x) + ")(" + int_to_str(y)
									+ ")_" + "L_flit_in";
							sc_trace(wf, dcNoc->tile[x][y]->tx_flit_out.read(),
									label);

							label = "(" + int_to_str(x) + ")(" + int_to_str(y)
									+ ")_" + "L_valid_in";
							sc_trace(wf, dcNoc->tile[x][y]->tx_valid_out,
									label);

							for (int k = 0; k < RouterParameter::n_VCs; k++) {
								label = "(" + int_to_str(x) + ")("
										+ int_to_str(y) + ")_"
										+ "L_in_vc_buffer_rd(" + int_to_str(k)
										+ ")";
								sc_trace(wf,
										dcNoc->tile[x][y]->tx_out_vc_buffer_rd[k],
										label);
							}

							label = "(" + int_to_str(x) + ")(" + int_to_str(y)
									+ ")_" + "L_flit_out";
							sc_trace(wf, dcNoc->tile[x][y]->rx_flit_in.read(),
									label);

							label = "(" + int_to_str(x) + ")(" + int_to_str(y)
									+ ")_" + "L_valid_out";
							sc_trace(wf, dcNoc->tile[x][y]->rx_valid_in, label);

							for (int k = 0; k < RouterParameter::n_VCs; k++) {
								label = "(" + int_to_str(x) + ")("
										+ int_to_str(y) + ")_"
										+ "L_out_vc_buffer_rd(" + int_to_str(k)
										+ ")";
								sc_trace(wf,
										dcNoc->tile[x][y]->rx_in_vc_buffer_rd[k],
										label);
							}
						}

						else {
							label = "(" + int_to_str(x) + ")(" + int_to_str(y)
									+ ")_" + direction + "_flit_in";
							sc_trace(wf, dcNoc->tile[x][y]->flit_in[i].read(),
									label);

							label = "(" + int_to_str(x) + ")(" + int_to_str(y)
									+ ")_" + direction + "_valid_in";
							sc_trace(wf, dcNoc->tile[x][y]->valid_in[i], label);

							for (int k = 0; k < RouterParameter::n_VCs; k++) {
								label = "(" + int_to_str(x) + ")("
										+ int_to_str(y) + ")_" + direction
										+ "_in_vc_buffer_rd(" + int_to_str(k)
										+ ")";
								sc_trace(wf,
										dcNoc->tile[x][y]->in_vc_buffer_rd[i][k],
										label);
							}

							label = "(" + int_to_str(x) + ")(" + int_to_str(y)
									+ ")_" + direction + "_flit_out";
							sc_trace(wf, dcNoc->tile[x][y]->flit_out[i].read(),
									label);

							label = "(" + int_to_str(x) + ")(" + int_to_str(y)
									+ ")_" + direction + "_valid_out";
							sc_trace(wf, dcNoc->tile[x][y]->valid_out[i],
									label);

							for (int k = 0; k < RouterParameter::n_VCs; k++) {
								label = "(" + int_to_str(x) + ")("
										+ int_to_str(y) + ")_" + direction
										+ "_out_vc_buffer_rd(" + int_to_str(k)
										+ ")";
								sc_trace(wf,
										dcNoc->tile[x][y]->out_vc_buffer_rd[i][k],
										label);
							}
						}
					}

					// Trace routers input ports internal signals
					string label;
					for (int pi = 0; pi < N_ROUTER_PORTS; pi++) {
						label = "(" + int_to_str(x) + ")(" + int_to_str(y)
								+ ")_" + int2direction(pi) + "_in_port_state";
						sc_trace(wf,
								wh_pipeline[x][y]->in_port_state[pi].read(),
								label);

						label = "(" + int_to_str(x) + ")(" + int_to_str(y)
								+ ")_" + int2direction(pi) + "_sa_allocated";
						sc_trace(wf, wh_pipeline[x][y]->sa_allocated[pi].read(),
								label);

						label = "(" + int_to_str(x) + ")(" + int_to_str(y)
								+ ")_" + int2direction(pi)
								+ "_in_port_out_port_req";
						sc_trace(wf, wh_pipeline[x][y]->out_port_req[pi].read(),
								label);

						label = "(" + int_to_str(x) + ")(" + int_to_str(y)
								+ ")_" + int2direction(pi)
								+ "_in_port_out_port_req";
						sc_trace(wf, wh_pipeline[x][y]->out_port_req[pi].read(),
								label);
					}

					// Trace routers output ports internal signals
					for (int po = 0; po < N_ROUTER_PORTS; po++) {
						label = "(" + int_to_str(x) + ")(" + int_to_str(y)
								+ ")_" + int2direction(po) + "_out_port_state";
						sc_trace(wf,
								wh_pipeline[x][y]->out_port_state[po].read(),
								label);

						label = "(" + int_to_str(x) + ")(" + int_to_str(y)
								+ ")_" + int2direction(po) + "_sa_grant";
						sc_trace(wf, wh_pipeline[x][y]->sa_grant[po].read(),
								label);

						label = "(" + int_to_str(x) + ")(" + int_to_str(y)
								+ ")_" + int2direction(po)
								+ "_out_credit_remain";
						sc_trace(wf,
								wh_pipeline[x][y]->out_credit_remain[po].read(),
								label);

						label = "(" + int_to_str(x) + ")(" + int_to_str(y)
								+ ")_" + int2direction(po) + "_credit_plus";
						sc_trace(wf, wh_pipeline[x][y]->credit_plus[po].read(),
								label);

						label = "(" + int_to_str(x) + ")(" + int_to_str(y)
								+ ")_" + int2direction(po) + "_credit_minus";
						sc_trace(wf, wh_pipeline[x][y]->credit_minus[po].read(),
								label);

						label = "(" + int_to_str(x) + ")(" + int_to_str(y)
								+ ")_" + int2direction(po) + "_tail_out_reg";
						sc_trace(wf, wh_pipeline[x][y]->tail_out_reg[po].read(),
								label);
					}
				}
			}
		}

		//cerr << "Starting SIMU " << endl;
		sc_start();

		// Close trace file
		if (DCParameters::generateWaveforms) {
			sc_close_vcd_trace_file(wf);
		}
	}

	cout << endl << endl;

	//################ Evaluation
	if (CommonParameter::platform_type != PLATFORM_DC) {
		cout << "================= PERFORMANCE RESULTS ================="
				<< endl;

		cout.precision(3);
		cout.setf(ios::fixed, ios::floatfield);   // floatfield set to fixed
	}

	if (CommonParameter::platform_type == PLATFORM_SYNTHETIC) {
		SyntheticEvaluation *e = new SyntheticEvaluation(noc);

		cout << "Average network latency = " << e->avg_latency_cal()
				<< " (cycles) = "
				<< e->avg_latency_cal() / CommonParameter::operating_clk_freq
				<< " (us) = "
				<< e->avg_latency_cal() / CommonParameter::operating_clk_freq
						* 1000 << " (ns)" << endl;

		double mean_packet_length;
		if (ProcessorParameters::packet_length_type == PACKET_LENGTH_TYPE_FIXED)
			mean_packet_length = (double) ProcessorParameters::packet_length;
		else
			mean_packet_length =
					(double) (ProcessorParameters::min_packet_length
							+ ProcessorParameters::max_packet_length) / 2;

		cout << "Average network throughput = " << e->avg_throughput_cal()
				<< " (packets/cycle) = "
				<< e->avg_throughput_cal() * mean_packet_length
				<< " (flits/cycle) = "
				<< e->avg_throughput_cal() * mean_packet_length
						* CommonParameter::operating_clk_freq
				<< " (flits/us) = "
				<< e->avg_throughput_cal() * mean_packet_length
						* CommonParameter::operating_clk_freq / 1000
				<< " (flits/ns)" << endl;
		delete (e);
	}

	else if (CommonParameter::platform_type == PLATFORM_EMBEDDED) {
		EmbeddedEvaluation *e = new EmbeddedEvaluation(noc);

		cout << "Average network latency = " << e->avg_latency_cal()
				<< " (cycles) = "
				<< e->avg_latency_cal() / CommonParameter::operating_clk_freq
				<< " (us) = "
				<< e->avg_latency_cal() / CommonParameter::operating_clk_freq
						* 1000 << " (ns)" << endl;

		double mean_packet_length;
		if (ProcessorParameters::packet_length_type == PACKET_LENGTH_TYPE_FIXED)
			mean_packet_length = (double) ProcessorParameters::packet_length;
		else
			mean_packet_length =
					(double) (ProcessorParameters::min_packet_length
							+ ProcessorParameters::max_packet_length) / 2;

		cout << "Average network throughput = " << e->avg_throughput_cal()
				<< " (packets/cycle) = "
				<< e->avg_throughput_cal() * mean_packet_length
				<< " (flits/cycle) = "
				<< e->avg_throughput_cal() * mean_packet_length
						* CommonParameter::operating_clk_freq
				<< " (flits/us) = "
				<< e->avg_throughput_cal() * mean_packet_length
						* CommonParameter::operating_clk_freq / 1000
				<< " (flits/ns)" << endl;

		delete (e);
	}

	if (CommonParameter::platform_type != PLATFORM_DC) {
		cout << "------------------------------------------------------"
				<< endl;

		cout << "Total number of received packets after warmup = "
				<< GlobalVariables::n_total_rx_packets << endl;
		cout << "Simulation stop at cycle = "
				<< GlobalVariables::last_simulation_time << "th" << endl;

		// display the overall power and energy
		power_energy_cal(noc);

		delete (noc);

		cout << "==============================================" << endl;

		time_t end_run = time(0);
		cout << "end at: " << ctime(&end_run);

		clock_t end_system_clock = clock();
		cout << "end at system clock: " << end_system_clock << "th" << endl;

		cout << "total simulation time = " << difftime(end_run, begin_run)
				<< " (seconds)" << "; or "
				<< difftime(end_system_clock, begin_system_clock)
				<< " (native system clock cycles)" << endl;
	}

	return 0;
}

/*
 * global clock tree + PLL power
 */
void global_clock_tree_power_cal() {
	// for 1, 2, 4, 8, 16, 32, 64 router cases

	double n_clk_buffer_x16[7] = { 0, 1, 1, 2, 4, 8, 16 };
	double n_clk_buffer_x32[7] = { 0, 0, 0, 1, 1, 2, 4 };
	double n_clk_buffer_x64[7] = { 0, 0, 0, 0, 0, 1, 1 };
	double n_clk_wire_x1[7] = { 0, 1, 3, 6, 12, 24, 48 };
	double n_clk_wire_x2[7] = { 0, 0, 0, 2, 3, 6, 12 };
	double n_clk_wire_x4[7] = { 0, 0, 0, 0, 0, 2, 3 };

	double n_tree_loads = CommonParameter::dim_x * CommonParameter::dim_y; // = n_routers
	int n = (int) ceil(log2(n_tree_loads));	// index

	double k = n_tree_loads / (1 << n);

	double n_tree_buffer_x16 = ceil(k * n_clk_buffer_x16[n]);
	double n_tree_buffer_x32 = ceil(k * n_clk_buffer_x32[n]);
	double n_tree_buffer_x64 = ceil(k * n_clk_buffer_x64[n]);

	double n_tree_wire_x1 = ceil(k * n_clk_wire_x1[n]);
	double n_tree_wire_x2 = ceil(k * n_clk_wire_x2[n]);
	double n_tree_wire_x4 = ceil(k * n_clk_wire_x4[n]);

	PowerParameter_65nm::global_clock_tree =
			n_tree_loads * STM65_POWER_CLOCK_BUFFER_X4
					+ n_tree_buffer_x16 * STM65_POWER_CLOCK_BUFFER_X16
					+ n_tree_buffer_x32 * STM65_POWER_CLOCK_BUFFER_X32
					+ n_tree_buffer_x64 * STM65_POWER_CLOCK_BUFFER_X64
					+ n_tree_wire_x1 * PTM65_POWER_CLOCK_WIRE_1000um_X1
							* (RouterParameter::link_length / 1000)+ n_tree_wire_x2 * PTM65_POWER_CLOCK_WIRE_2000um_X2 * (RouterParameter::link_length/1000)
							+ n_tree_wire_x4 * PTM65_POWER_CLOCK_WIRE_4000um_X4 * (RouterParameter::link_length/1000)
							+ GENERAL65_POWER_PLL_ACTIVE;
}

/*
 * timing and power regulation according to pipeline type and voltage
 */
void timing_power_regulation() {
	//---------- adjust timing following voltage
	TimingParameter_65nm::setup = TimingParameter_65nm::setup * 1.3
			/ CommonParameter::input_voltage;
	TimingParameter_65nm::clk_q = TimingParameter_65nm::clk_q * 1.3
			/ CommonParameter::input_voltage;
	TimingParameter_65nm::BW = TimingParameter_65nm::BW * 1.3
			/ CommonParameter::input_voltage;
	TimingParameter_65nm::RC = TimingParameter_65nm::RC * 1.3
			/ CommonParameter::input_voltage;
	TimingParameter_65nm::SA = TimingParameter_65nm::SA * 1.3
			/ CommonParameter::input_voltage;
	TimingParameter_65nm::ST = TimingParameter_65nm::ST * 1.3
			/ CommonParameter::input_voltage;
	TimingParameter_65nm::LT = TimingParameter_65nm::LT * 1.3
			/ CommonParameter::input_voltage;

	//---------- adjust min clock period of the router following pipeline
	double min_clk_period;

	double max_RC_SA = max(TimingParameter_65nm::RC, TimingParameter_65nm::SA);

	switch (RouterParameter::pipeline_type) {
	case PIPELINE_TYPE_1:
		min_clk_period = TimingParameter_65nm::clk_q
				+ TimingParameter_65nm::setup + TimingParameter_65nm::BW
				+ max_RC_SA + TimingParameter_65nm::ST
				+ TimingParameter_65nm::LT;
		break;
	case PIPELINE_TYPE_2_1:
		min_clk_period = TimingParameter_65nm::clk_q
				+ TimingParameter_65nm::setup
				+ max(TimingParameter_65nm::BW,
						max_RC_SA + TimingParameter_65nm::ST
								+ TimingParameter_65nm::LT);
		break;
	case PIPELINE_TYPE_2_2:
		min_clk_period = TimingParameter_65nm::clk_q
				+ TimingParameter_65nm::setup
				+ max(TimingParameter_65nm::BW + max_RC_SA,
						TimingParameter_65nm::ST + TimingParameter_65nm::LT);
		break;
	case PIPELINE_TYPE_2_3:
		min_clk_period = TimingParameter_65nm::clk_q
				+ TimingParameter_65nm::setup
				+ max(
						TimingParameter_65nm::BW + max_RC_SA
								+ TimingParameter_65nm::ST,
						TimingParameter_65nm::LT);
		break;
	case PIPELINE_TYPE_3_1:
		min_clk_period = TimingParameter_65nm::clk_q
				+ TimingParameter_65nm::setup
				+ max(max(TimingParameter_65nm::BW, max_RC_SA),
						TimingParameter_65nm::ST + TimingParameter_65nm::LT);
		break;
	case PIPELINE_TYPE_3_2:
		min_clk_period = TimingParameter_65nm::clk_q
				+ TimingParameter_65nm::setup
				+ max(
						max(TimingParameter_65nm::BW,
								max_RC_SA + TimingParameter_65nm::ST),
						TimingParameter_65nm::LT);
		break;
	case PIPELINE_TYPE_3_3:
		min_clk_period = TimingParameter_65nm::clk_q
				+ TimingParameter_65nm::setup
				+ max(
						max(TimingParameter_65nm::BW + max_RC_SA,
								TimingParameter_65nm::ST),
						TimingParameter_65nm::LT);

		break;
	case PIPELINE_TYPE_4:
		min_clk_period = TimingParameter_65nm::clk_q
				+ TimingParameter_65nm::setup
				+ max(
						max(max(TimingParameter_65nm::BW, max_RC_SA),
								TimingParameter_65nm::ST),
						TimingParameter_65nm::LT);
		break;
	default:
		cout << "ERROR: pipeline type is not supported!" << endl;
		exit(0);
	}

	double max_clk_freq = 1000000 / min_clk_period; // (MHz)

	//----------- regulate power according to the operating clk rate and voltage
	double operating_clk_freq;
	if (CommonParameter::clk_freq_mode == CLK_FREQ_FIXED) {
		operating_clk_freq = CommonParameter::input_clk_freq;
	} else {
		operating_clk_freq = max_clk_freq;
	}

	CommonParameter::operating_clk_freq = operating_clk_freq;

	double clk_freq_ratio = operating_clk_freq / 1000; // default clk_rate = 1GHz = 1000 MHz
	double voltage_ratio = CommonParameter::input_voltage / 1.3; // default voltage = 1.3V

	double scale = (voltage_ratio * voltage_ratio) * clk_freq_ratio; // P ~ C(V^2)f

	PowerParameter_65nm::buffer_34b_rd_wr =
			PowerParameter_65nm::buffer_34b_rd_wr * scale;
	PowerParameter_65nm::buffer_34b_rd_wr_clk =
			PowerParameter_65nm::buffer_34b_rd_wr_clk * scale;
	PowerParameter_65nm::buffer_34b_rd_wr_reg =
			PowerParameter_65nm::buffer_34b_rd_wr_reg * scale;

	PowerParameter_65nm::buffer_34b_rd_only =
			PowerParameter_65nm::buffer_34b_rd_only * scale;
	PowerParameter_65nm::buffer_34b_rd_only_clk =
			PowerParameter_65nm::buffer_34b_rd_only_clk * scale;
	PowerParameter_65nm::buffer_34b_rd_only_reg =
			PowerParameter_65nm::buffer_34b_rd_only_reg * scale;

	PowerParameter_65nm::buffer_34b_wr_only =
			PowerParameter_65nm::buffer_34b_wr_only * scale;
	PowerParameter_65nm::buffer_34b_wr_only_clk =
			PowerParameter_65nm::buffer_34b_wr_only_clk * scale;
	PowerParameter_65nm::buffer_34b_wr_only_reg =
			PowerParameter_65nm::buffer_34b_wr_only_reg * scale;

	// TODO: find a better scaling rule for leakage power
	// even leakage is small, leakeage is not proportial to (V^2)f
	// in this version: leakage is proportional to voltage
	PowerParameter_65nm::buffer_34b_leak = PowerParameter_65nm::buffer_34b_leak
			* voltage_ratio;
//	PowerParameter_65nm::buffer_34b_inactive = PowerParameter_65nm::buffer_34b_inactive * scale;
	PowerParameter_65nm::buffer_34b_inactive_clk =
			PowerParameter_65nm::buffer_34b_inactive_clk * scale;
	PowerParameter_65nm::buffer_34b_inactive_reg =
			PowerParameter_65nm::buffer_34b_inactive_reg * scale;
	PowerParameter_65nm::buffer_34b_inactive =
			PowerParameter_65nm::buffer_34b_inactive_clk
					+ PowerParameter_65nm::buffer_34b_inactive_reg
					+ PowerParameter_65nm::buffer_34b_leak;

	PowerParameter_65nm::routing_12b_head =
			PowerParameter_65nm::routing_12b_head * scale;
	PowerParameter_65nm::routing_12b_head_clk =
			PowerParameter_65nm::routing_12b_head_clk * scale;
	PowerParameter_65nm::routing_12b_head_reg =
			PowerParameter_65nm::routing_12b_head_reg * scale;

	PowerParameter_65nm::routing_12b_head_stall =
			PowerParameter_65nm::routing_12b_head_stall * scale;
	PowerParameter_65nm::routing_12b_head_stall_clk =
			PowerParameter_65nm::routing_12b_head_stall_clk * scale;
	PowerParameter_65nm::routing_12b_head_stall_reg =
			PowerParameter_65nm::routing_12b_head_stall_reg * scale;

	PowerParameter_65nm::routing_12b_body =
			PowerParameter_65nm::routing_12b_body * scale;
	PowerParameter_65nm::routing_12b_body_clk =
			PowerParameter_65nm::routing_12b_body_clk * scale;
	PowerParameter_65nm::routing_12b_body_reg =
			PowerParameter_65nm::routing_12b_body_reg * scale;

	PowerParameter_65nm::routing_12b_leak =
			PowerParameter_65nm::routing_12b_leak * voltage_ratio;
//	PowerParameter_65nm::routing_12b_inactive = PowerParameter_65nm::routing_12b_inactive * scale;
	PowerParameter_65nm::routing_12b_inactive_clk =
			PowerParameter_65nm::routing_12b_inactive_clk * scale;
	PowerParameter_65nm::routing_12b_inactive_reg =
			PowerParameter_65nm::routing_12b_inactive_reg * scale;
	PowerParameter_65nm::routing_12b_inactive =
			PowerParameter_65nm::routing_12b_inactive_clk
					+ PowerParameter_65nm::routing_12b_inactive_reg
					+ PowerParameter_65nm::routing_12b_leak;

	PowerParameter_65nm::arbiter_4inputs_active =
			PowerParameter_65nm::arbiter_4inputs_active * scale;
	PowerParameter_65nm::arbiter_4inputs_active_clk =
			PowerParameter_65nm::arbiter_4inputs_active_clk * scale;
	PowerParameter_65nm::arbiter_4inputs_active_reg =
			PowerParameter_65nm::arbiter_4inputs_active_reg * scale;

	PowerParameter_65nm::arbiter_4inputs_hold =
			PowerParameter_65nm::arbiter_4inputs_hold * scale;
	PowerParameter_65nm::arbiter_4inputs_hold_clk =
			PowerParameter_65nm::arbiter_4inputs_hold_clk * scale;
	PowerParameter_65nm::arbiter_4inputs_hold_reg =
			PowerParameter_65nm::arbiter_4inputs_hold_reg * scale;

	PowerParameter_65nm::arbiter_4inputs_not_ready =
			PowerParameter_65nm::arbiter_4inputs_not_ready * scale;
	PowerParameter_65nm::arbiter_4inputs_not_ready_clk =
			PowerParameter_65nm::arbiter_4inputs_not_ready_clk * scale;
	PowerParameter_65nm::arbiter_4inputs_not_ready_reg =
			PowerParameter_65nm::arbiter_4inputs_not_ready_reg * scale;

	PowerParameter_65nm::arbiter_4inputs_leak =
			PowerParameter_65nm::arbiter_4inputs_leak * voltage_ratio;
//	PowerParameter_65nm::arbiter_4inputs_inactive = PowerParameter_65nm::arbiter_4inputs_inactive * scale;
	PowerParameter_65nm::arbiter_4inputs_inactive_clk =
			PowerParameter_65nm::arbiter_4inputs_inactive_clk * scale;
	PowerParameter_65nm::arbiter_4inputs_inactive_reg =
			PowerParameter_65nm::arbiter_4inputs_inactive_reg * scale;
	PowerParameter_65nm::arbiter_4inputs_inactive =
			PowerParameter_65nm::arbiter_4inputs_inactive_clk
					+ PowerParameter_65nm::arbiter_4inputs_inactive_reg
					+ PowerParameter_65nm::arbiter_4inputs_leak;

	PowerParameter_65nm::out_credit_counter_5b_one_on =
			PowerParameter_65nm::out_credit_counter_5b_one_on * scale;
	PowerParameter_65nm::out_credit_counter_5b_one_on_clk =
			PowerParameter_65nm::out_credit_counter_5b_one_on_clk * scale;
	PowerParameter_65nm::out_credit_counter_5b_one_on_reg =
			PowerParameter_65nm::out_credit_counter_5b_one_on_reg * scale;

	PowerParameter_65nm::out_credit_counter_5b_one_changes =
			PowerParameter_65nm::out_credit_counter_5b_one_changes * scale;
	PowerParameter_65nm::out_credit_counter_5b_one_changes_clk =
			PowerParameter_65nm::out_credit_counter_5b_one_changes_clk * scale;
	PowerParameter_65nm::out_credit_counter_5b_one_changes_reg =
			PowerParameter_65nm::out_credit_counter_5b_one_changes_reg * scale;

	PowerParameter_65nm::out_credit_counter_5b_both_change_diff =
			PowerParameter_65nm::out_credit_counter_5b_both_change_diff * scale;
	PowerParameter_65nm::out_credit_counter_5b_both_change_diff_clk =
			PowerParameter_65nm::out_credit_counter_5b_both_change_diff_clk
					* scale;
	PowerParameter_65nm::out_credit_counter_5b_both_change_diff_reg =
			PowerParameter_65nm::out_credit_counter_5b_both_change_diff_reg
					* scale;

	PowerParameter_65nm::out_credit_counter_5b_leak =
			PowerParameter_65nm::out_credit_counter_5b_leak * voltage_ratio;
//	PowerParameter_65nm::out_credit_counter_5b_inactive = PowerParameter_65nm::out_credit_counter_5b_inactive * scale;
	PowerParameter_65nm::out_credit_counter_5b_inactive_clk =
			PowerParameter_65nm::out_credit_counter_5b_inactive_clk * scale;
	PowerParameter_65nm::out_credit_counter_5b_inactive_reg =
			PowerParameter_65nm::out_credit_counter_5b_inactive_reg * scale;
	PowerParameter_65nm::out_credit_counter_5b_inactive =
			PowerParameter_65nm::out_credit_counter_5b_inactive_clk
					+ PowerParameter_65nm::out_credit_counter_5b_inactive_reg
					+ PowerParameter_65nm::out_credit_counter_5b_leak;

	PowerParameter_65nm::crossbar_34b_one_active =
			PowerParameter_65nm::crossbar_34b_one_active * scale;
//	PowerParameter_65nm::crossbar_34b_inactive = PowerParameter_65nm::crossbar_34b_inactive * scale;
	PowerParameter_65nm::crossbar_34b_inactive =
			PowerParameter_65nm::crossbar_34b_inactive * voltage_ratio;
//	PowerParameter_65nm::crossbar_34b_leak = PowerParameter_65nm::crossbar_34b_leak * scale;
	PowerParameter_65nm::crossbar_34b_leak =
			PowerParameter_65nm::crossbar_34b_leak * voltage_ratio;

	PowerParameter_65nm::link_1000um_34b_active =
			PowerParameter_65nm::link_1000um_34b_active * scale;
//	PowerParameter_65nm::link_1000um_34b_inactive = PowerParameter_65nm::link_1000um_34b_inactive * scale;
	PowerParameter_65nm::link_1000um_34b_inactive =
			PowerParameter_65nm::link_1000um_34b_inactive * voltage_ratio;
//	PowerParameter_65nm::link_1000um_34b_leak = PowerParameter_65nm::link_1000um_34b_leak * scale;
	PowerParameter_65nm::link_1000um_34b_leak =
			PowerParameter_65nm::link_1000um_34b_leak * voltage_ratio;

//	static double link_500um_34b_active;
//	static double link_500um_34b_inactive;
//	static double link_500um_34b_leak;

	PowerParameter_65nm::pipeline_34b_active =
			PowerParameter_65nm::pipeline_34b_active * scale;
	PowerParameter_65nm::pipeline_34b_active_clk =
			PowerParameter_65nm::pipeline_34b_active_clk * scale;
	PowerParameter_65nm::pipeline_34b_active_reg =
			PowerParameter_65nm::pipeline_34b_active_reg * scale;

	PowerParameter_65nm::pipeline_34b_leak =
			PowerParameter_65nm::pipeline_34b_leak * voltage_ratio;
//	PowerParameter_65nm::pipeline_34b_inactive = PowerParameter_65nm::pipeline_34b_inactive * scale;
	PowerParameter_65nm::pipeline_34b_inactive_clk =
			PowerParameter_65nm::pipeline_34b_inactive_clk * scale;
	PowerParameter_65nm::pipeline_34b_inactive_reg =
			PowerParameter_65nm::pipeline_34b_inactive_reg * scale;
	PowerParameter_65nm::pipeline_34b_inactive =
			PowerParameter_65nm::pipeline_34b_inactive_clk
					+ PowerParameter_65nm::pipeline_34b_inactive_reg
					+ PowerParameter_65nm::pipeline_34b_leak;

	PowerParameter_65nm::pipeline_5b_active =
			PowerParameter_65nm::pipeline_5b_active * scale;
	PowerParameter_65nm::pipeline_5b_active_clk =
			PowerParameter_65nm::pipeline_5b_active_clk * scale;
	PowerParameter_65nm::pipeline_5b_active_reg =
			PowerParameter_65nm::pipeline_5b_active_reg * scale;

	PowerParameter_65nm::pipeline_5b_leak =
			PowerParameter_65nm::pipeline_5b_leak * voltage_ratio;
//	PowerParameter_65nm::pipeline_5b_inactive = PowerParameter_65nm::pipeline_5b_inactive * scale;
	PowerParameter_65nm::pipeline_5b_inactive_clk =
			PowerParameter_65nm::pipeline_5b_inactive_clk * scale;
	PowerParameter_65nm::pipeline_5b_inactive_reg =
			PowerParameter_65nm::pipeline_5b_inactive_reg * scale;
	PowerParameter_65nm::pipeline_5b_inactive =
			PowerParameter_65nm::pipeline_5b_inactive_clk
					+ PowerParameter_65nm::pipeline_5b_inactive_reg
					+ PowerParameter_65nm::pipeline_5b_leak;

	PowerParameter_65nm::global_clock_tree =
			PowerParameter_65nm::global_clock_tree * scale;

}

/*
 * power_energy_cal
 */
void power_energy_cal(Platform *platform) {
	cout << "=========== POWER & ENERGY RESULTS (32-bit datapath) ==========="
			<< endl;
	cout << "=========== AT " << CommonParameter::input_voltage << "V, "
			<< CommonParameter::operating_clk_freq << "MHz =========" << endl;

	WormholePipelinePowerEstimation *power =
			new WormholePipelinePowerEstimation(platform);

	cout << "-------------------------------------------------------" << endl;
	cout << "Average router power (both dynamic & leakage) = "
			<< power->get_avg_power() << " (uW); " << endl;

	cout << "--- clock power = " << power->get_avg_clk_power() << " (uW); "
			<< "reg. power = " << power->get_avg_reg_power() << " (uW); "
			<< "comb. power = "
			<< power->get_avg_power() - power->get_avg_clk_power()
					- power->get_avg_reg_power() << " (uW)" << endl;

	cout << "++++++ average buffer power = " << power->get_buffer_avg_power()
			<< " (uW)" << endl;
	cout << "-------- buffer clk = " << power->get_buffer_avg_clk_power()
			<< " (uW); " << "buffer reg. = "
			<< power->get_buffer_avg_reg_power() << " (uW); "
			<< "buffer comb. = "
			<< power->get_buffer_avg_power() - power->get_buffer_avg_clk_power()
					- power->get_buffer_avg_reg_power() << " (uW)" << endl;

	cout << "++++++ average routing power = " << power->get_routing_avg_power()
			<< " (uW)" << endl;
	cout << "-------- routing clk = " << power->get_routing_avg_clk_power()
			<< " (uW); " << "routing reg. = "
			<< power->get_routing_avg_reg_power() << " (uW); "
			<< "routing comb. = "
			<< power->get_routing_avg_power()
					- power->get_routing_avg_clk_power()
					- power->get_routing_avg_reg_power() << " (uW)" << endl;

	cout << "++++++ average switch arbitration power = "
			<< power->get_sa_avg_power() << " (uW)" << endl;
	cout << "-------- arbitration clk = " << power->get_sa_avg_clk_power()
			<< " (uW); " << "arbitration reg. = "
			<< power->get_sa_avg_reg_power() << " (uW); "
			<< "arbitration comb. = "
			<< power->get_sa_avg_power() - power->get_sa_avg_clk_power()
					- power->get_sa_avg_reg_power() << " (uW)" << endl;

	cout << "++++++ average crossbar power = "
			<< power->get_crossbar_avg_power() << " (uW)" << endl;

	cout << "++++++ average out_credit_counter power = "
			<< power->get_out_credit_counter_avg_power() << " (uW)" << endl;
	cout << "-------- credit_counter clk = "
			<< power->get_out_credit_counter_avg_clk_power() << " (uW); "
			<< "credit_counter reg. = "
			<< power->get_out_credit_counter_avg_reg_power() << " (uW); "
			<< "credit_counter comb. = "
			<< power->get_out_credit_counter_avg_power()
					- power->get_out_credit_counter_avg_clk_power()
					- power->get_out_credit_counter_avg_reg_power() << " (uW)"
			<< endl;

	cout << "++++++ average link power = " << power->get_link_avg_power()
			<< " (uW)" << endl;

	cout << "++++++ average pipeline_datapath power = "
			<< power->get_pipeline_datapath_avg_power() << " (uW)" << endl;
	cout << "-------- pipeline_datapath clk = "
			<< power->get_pipeline_datapath_avg_clk_power() << " (uW); "
			<< "pipeline_datapath reg. = "
			<< power->get_pipeline_datapath_avg_reg_power() << " (uW); "
			<< "pipeline_datapath comb. = "
			<< power->get_pipeline_datapath_avg_power()
					- power->get_pipeline_datapath_avg_clk_power()
					- power->get_pipeline_datapath_avg_reg_power() << " (uW)"
			<< endl;

	cout << "++++++ average pipeline_control power = "
			<< power->get_pipeline_control_avg_power() << " (uW)" << endl;
	cout << "-------- pipeline_control clk = "
			<< power->get_pipeline_control_avg_clk_power() << " (uW); "
			<< "pipeline_control reg. = "
			<< power->get_pipeline_control_avg_reg_power() << " (uW); "
			<< "pipeline_control comb. = "
			<< power->get_pipeline_control_avg_power()
					- power->get_pipeline_control_avg_clk_power()
					- power->get_pipeline_control_avg_reg_power() << " (uW)"
			<< endl;

	cout << "==========================================" << endl;

	cout << "Leakage router power = " << power->get_avg_leak_power()
			<< " (uW), " << " or "
			<< power->get_avg_leak_power() / power->get_avg_power() * 100
			<< "% of the average power" << endl;

	cout
			<< "Inactive router power (average power at fir of 0 = router leakage + local router clock) = "
			<< power->get_avg_inactive_power() << " (uW)" << endl;

	cout << "==========================================" << endl;

	cout << "Total power of all routers = "
			<< power->get_avg_power() * CommonParameter::dim_x
					* CommonParameter::dim_y << " (uW)" << endl;

	cout
			<< "Global clock tree power + PLL (don't count subtrees inside routers) = "
			<< PowerParameter_65nm::global_clock_tree << " (uW)" << endl;

	cout << "==> Total network power (routers and global clock tree) = "
			<< PowerParameter_65nm::global_clock_tree
					+ power->get_avg_power() * CommonParameter::dim_x
							* CommonParameter::dim_y << " (uW) = "
			<< (PowerParameter_65nm::global_clock_tree
					+ power->get_avg_power() * CommonParameter::dim_x
							* CommonParameter::dim_y) / 1000 << " (mW)" << endl;

	double global_clk_tree_energy_per_packet =
			PowerParameter_65nm::global_clock_tree
					* (GlobalVariables::last_simulation_time
							- CommonParameter::warmup_time)
					/ GlobalVariables::n_total_rx_packets
					/ CommonParameter::operating_clk_freq;

	double total_energy_per_packet = power->get_avg_energy_per_packet()
			* CommonParameter::dim_x * CommonParameter::dim_y
			+ global_clk_tree_energy_per_packet;

	double mean_packet_length;
	if (ProcessorParameters::packet_length_type == PACKET_LENGTH_TYPE_FIXED)
		mean_packet_length = (double) ProcessorParameters::packet_length;
	else
		mean_packet_length = (double) (ProcessorParameters::min_packet_length
				+ ProcessorParameters::max_packet_length) / 2;

	cout << "Total network energy per packet = " << total_energy_per_packet
			<< " (pJ/packet) = " << total_energy_per_packet / mean_packet_length
			<< " (pJ/flit)" << endl;

	cout << "--- energy of all routers per packet  = "
			<< power->get_avg_energy_per_packet() * CommonParameter::dim_x
					* CommonParameter::dim_y << " (pJ/packet) = "
			<< power->get_avg_energy_per_packet() * CommonParameter::dim_x
					* CommonParameter::dim_y / mean_packet_length
			<< " (pJ/flit)" << endl;

	cout << "--- global clock tree energy per packet = "
			<< global_clk_tree_energy_per_packet << " (pJ/packet) = "
			<< global_clk_tree_energy_per_packet / mean_packet_length
			<< " (pJ/flit)" << endl;

	cout << "==========================================" << endl;

	cout << "Average router energy per packet (including global clock) = "
			<< total_energy_per_packet
					/ (CommonParameter::dim_x * CommonParameter::dim_y)
			<< " (pJ/packet/router) = "
			<< total_energy_per_packet
					/ (CommonParameter::dim_x * CommonParameter::dim_y)
					/ mean_packet_length << " (pJ/flit/router)" << endl;
}
