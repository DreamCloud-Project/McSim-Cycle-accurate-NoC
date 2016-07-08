/*
 * console_parse_show.cpp
 *
 *  Created on: May 28, 2010
 *      Author: Anh Tran
 *
 *  implementation of console_parse_show.h
 */

#include "console_parse_show.h"
#include <sys/stat.h>

// Command line parser
void command_line_parser(int argc, char *argv[]) {
	if (argc == 1)
		cout << "Using default network settings" << endl;
	else {
		string option;
		for (int i = 1; i <= argc - 1; i++) {
			option = argv[i];
			if (!option.compare("-help")) {
				show_help();
				exit(0);
			} else if (!option.compare("-volt")) {
				CommonParameter::input_voltage = atof(argv[++i]);
			} else if (!option.compare("-freqmode")) {
				string freq_mode = argv[++i];
				if (!freq_mode.compare("fixed")) {
					CommonParameter::clk_freq_mode = CLK_FREQ_FIXED;
				} else if (!freq_mode.compare("max")) {
					CommonParameter::clk_freq_mode = CLK_FREQ_MAX;
				} else {
					cout << "The clk frequency mode is invalid!" << endl;
					exit(0);
				}
			} else if (!option.compare("-freq")) {
				CommonParameter::input_clk_freq = atof(argv[++i]);
			} else if (!option.compare("-router")) {
				string router_type = argv[++i];
				if (!router_type.compare("wormhole"))
					RouterParameter::router_type = ROUTER_WORMHOLE;
				else if (!router_type.compare("vc"))
					RouterParameter::router_type = ROUTER_VC;
				else {
					cout << "The router type is invalid!" << endl;
					exit(0);
				}
			} else if (!option.compare("-llength")) {	// link length
				RouterParameter::link_length = atof(argv[++i]);
			} else if (!option.compare("-pptype")) {
				string pptype = argv[++i];
				if (!pptype.compare("1"))
					RouterParameter::pipeline_type = PIPELINE_TYPE_1;
				else if (!pptype.compare("2_1"))
					RouterParameter::pipeline_type = PIPELINE_TYPE_2_1;
				else if (!pptype.compare("2_2"))
					RouterParameter::pipeline_type = PIPELINE_TYPE_2_2;
				else if (!pptype.compare("2_3"))
					RouterParameter::pipeline_type = PIPELINE_TYPE_2_3;
				else if (!pptype.compare("3_1"))
					RouterParameter::pipeline_type = PIPELINE_TYPE_3_1;
				else if (!pptype.compare("3_2"))
					RouterParameter::pipeline_type = PIPELINE_TYPE_3_2;
				else if (!pptype.compare("3_3"))
					RouterParameter::pipeline_type = PIPELINE_TYPE_3_3;
				else if (!pptype.compare("4"))
					RouterParameter::pipeline_type = PIPELINE_TYPE_4;
				else {
					cout << "ERROR: The pipeline type is invalid!" << endl;
					exit(0);
				}
			}

			else if (!option.compare("-fir"))	// flit_injection_rate
				ProcessorParameters::flit_inject_rate = atof(argv[++i]);
			else if (!option.compare("-traffic")) {	// traffic type
				string traffic = argv[++i];
				if (!traffic.compare("random"))
					SyntheticParameters::traffic_type = TRAFFIC_RANDOM;	// default
				else if (!traffic.compare("transpose"))
					SyntheticParameters::traffic_type = TRAFFIC_TRANSPOSE;
				else if (!traffic.compare("bitc"))
					SyntheticParameters::traffic_type = TRAFFIC_BIT_COMPLEMENT;
				else if (!traffic.compare("bitr"))
					SyntheticParameters::traffic_type = TRAFFIC_BIT_REVERSAL;
				else if (!traffic.compare("tornado"))
					SyntheticParameters::traffic_type = TRAFFIC_TORNADO;
				else if (!traffic.compare("regional"))
					SyntheticParameters::traffic_type = TRAFFIC_REGIONAL;
				else if (!traffic.compare("neighbor"))
					SyntheticParameters::traffic_type = TRAFFIC_NEIGHBOR;
				else if (!traffic.compare("hotspot"))
					SyntheticParameters::traffic_type = TRAFFIC_HOTSPOT;
				else if (!traffic.compare("shuffle"))
					SyntheticParameters::traffic_type = TRAFFIC_SHUFFLE;
				else if (!traffic.compare("rotate"))
					SyntheticParameters::traffic_type = TRAFFIC_ROTATE;
				else {
					cout << "ERROR: The traffic type is invalid!" << endl;
					exit(0);
				}
			} else if (!option.compare("-nhs")) {	// n_hotspots
				SyntheticParameters::n_hotspots = atoi(argv[++i]);
			} else if (!option.compare("-hstype")) {
				string hotspot_type = argv[++i];
				if (!hotspot_type.compare("central"))
					SyntheticParameters::hotspot_type = HOTSPOT_CENTRAL;
				else if (!hotspot_type.compare("corner"))
					SyntheticParameters::hotspot_type = HOTSPOT_CORNER;
				else {
					cout << "ERROR: Hotspot type is invalid!" << endl;
					exit(0);
				}
			} else if (!option.compare("-percent")) {// traffic percentage for regional, neighbor or hotspot
				int percent = atoi(argv[++i]);
				switch (SyntheticParameters::traffic_type) {
				case (TRAFFIC_REGIONAL):
					SyntheticParameters::regional_percentage = percent;
					break;
				case (TRAFFIC_NEIGHBOR):
					SyntheticParameters::neighbor_percentage = percent;
					break;
				case (TRAFFIC_HOTSPOT):
					SyntheticParameters::hotspot_percentage = percent;
					break;
				default:
					;
				}
			}

			else if (!option.compare("-dist")) {
				string dist_type = argv[++i];
				if (!dist_type.compare("exponential"))
					ProcessorParameters::inter_arrival_time_type =
					INTER_ARRIVAL_EXPONENTIAL;	// default
				else if (!dist_type.compare("identical"))
					ProcessorParameters::inter_arrival_time_type =
					INTER_ARRIVAL_IDENTICAL;
				else {
					cout << "ERROR: The traffic type is invalid!" << endl;
					exit(0);
				}
			}

			else if (!option.compare("-plength")) {	// packet length type
				string plength_type = argv[++i];
				if (!plength_type.compare("fixed"))
					ProcessorParameters::packet_length_type =
					PACKET_LENGTH_TYPE_FIXED;
				else if (!plength_type.compare("variable"))
					ProcessorParameters::packet_length_type =
					PACKET_LENGTH_TYPE_VARIABLE;
				else {
					cout << "ERROR: the packet length type is invalid!" << endl;
					cout << "Please choose: fixed or variable." << endl;
					exit(0);
				}

			} else if (!option.compare("-length")) // fixed packet length
				ProcessorParameters::packet_length = atoi(argv[++i]);
			else if (!option.compare("-lengthmin"))
				ProcessorParameters::min_packet_length = atoi(argv[++i]);
			else if (!option.compare("-lengthmax"))
				ProcessorParameters::max_packet_length = atoi(argv[++i]);

			else if (!option.compare("-routing")) {	// routing algorithm
				string routing_type = argv[++i];
				if (!routing_type.compare("xy") || !routing_type.compare("XY"))
					RouterParameter::routing_type = ROUTING_XY;
				else if (!routing_type.compare("wfminimal")
						|| !routing_type.compare("WFminimal"))
					RouterParameter::routing_type = ROUTING_MINIMAL_WEST_FIRST;
				else if (!routing_type.compare("nfminimal")
						|| !routing_type.compare("NFminimal"))
					RouterParameter::routing_type =
					ROUTING_MINIMAL_NEGATIVE_FIRST;
				else if (!routing_type.compare("nlminimal")
						|| !routing_type.compare("NLminimal"))
					RouterParameter::routing_type = ROUTING_MINIMAL_NORTH_LAST;
				else if (!routing_type.compare("oeminimal")
						|| !routing_type.compare("OEminimal"))
					RouterParameter::routing_type = ROUTING_MINIMAL_ODD_EVEN;
				else {
					cout << "ERROR: routing type is not supported!" << endl;
					cout
							<< "Please choose: xy, wfminimal, nfminimal, nlminimal, oeminimal"
							<< endl;
					exit(0);
				}
			} else if (!option.compare("-outsel")) {// output select policy for adaptive routing
				string outselect_type = argv[++i];
				if (!outselect_type.compare("xyordered"))
					RouterParameter::output_channel_select =
					OUTPUT_CHANNEL_SELECT_XY_ORDERED;
				else if (!outselect_type.compare("nearerdim"))
					RouterParameter::output_channel_select =
					OUTPUT_CHANNEL_SELECT_NEARER_DIMENSION;
				else if (!outselect_type.compare("fartherdim"))
					RouterParameter::output_channel_select =
					OUTPUT_CHANNEL_SELECT_FARTHER_DIMENSION;
				else if (!outselect_type.compare("roundrobin"))
					RouterParameter::output_channel_select =
					OUTPUT_CHANNEL_SELECT_ROUNDROBIN;
				else if (!outselect_type.compare("highercredit"))
					RouterParameter::output_channel_select =
					OUTPUT_CHANNEL_SELECT_HIGHER_CREDIT;
				else {
					cout
							<< "ERROR: the output selection policy is not supported!"
							<< endl;
					cout
							<< "Please choose: xyordered, nearerdim, fartherdim, roundrobin, highercredit"
							<< endl;
					exit(0);
				}
			} else if (!option.compare("-plaware")) {
				int plaware = atoi(argv[++i]);
				if (plaware == 0)
					RouterParameter::packet_length_aware = 0;
				else
					RouterParameter::packet_length_aware = 1;
			}

			else if (!option.compare("-vca")) {
				string vca_type = argv[++i];
				if (!vca_type.compare("rr"))
					RouterParameter::vca_type = VCA_ROUNDROBIN;
				else if (!vca_type.compare("islip"))
					RouterParameter::vca_type = VCA_iSLIP;
				else {
					cout << "ERROR: the VCA algorithm option is invalid!"
							<< endl;
					exit(0);
				}
			} else if (!option.compare("-sa")) {
				string sa_type = argv[++i];
				if (!sa_type.compare("rr"))
					RouterParameter::sa_type = SA_ROUNDROBIN;
				else if (!sa_type.compare("islip"))
					RouterParameter::sa_type = SA_iSLIP;
				else if (!sa_type.compare("takeall"))
					RouterParameter::sa_type = SA_WINNER_TAKE_ALL;
				else {
					cout << "ERROR: the SA algorithm option is invalid!"
							<< endl;
					exit(0);
				}
			} else if (!option.compare("-sba")) {
				string sba_type = argv[++i];
				if (!sba_type.compare("rr"))
					RouterParameter::sba_type = SBA_ROUNDROBIN;
				else if (!sba_type.compare("islip"))
					RouterParameter::sba_type = SBA_iSLIP;
				else {
					cout << "ERROR: the SBA algorithm option is invalid!"
							<< endl;
					exit(0);
				}
			} else if (!option.compare("-sba_th")) {
				RouterParameter::sba_threshold = atoi(argv[++i]);
			} else if (!option.compare("-sba_reg")) {
				int sba_reg = atoi(argv[++i]);
				if (sba_reg == 0)
					RouterParameter::sba_regulated = 0;
				else
					RouterParameter::sba_regulated = 1;
			}

			else if (!option.compare("-bsize"))		// buffer size
				RouterParameter::buffer_size = atoi(argv[++i]);
			else if (!option.compare("-sbsize"))		// buffer size
				RouterParameter::shared_buffer_size = atoi(argv[++i]);
			else if (!option.compare("-nvc"))	// the number of VCs per port
				RouterParameter::n_VCs = atoi(argv[++i]);
			else if (!option.compare("-nsb"))		// the number of SBs
				RouterParameter::n_SBs = atoi(argv[++i]);
			else if (!option.compare("-dimx"))
				CommonParameter::dim_x = atoi(argv[++i]);
			else if (!option.compare("-dimy"))
				CommonParameter::dim_y = atoi(argv[++i]);
			else if (!option.compare("-seed"))
				CommonParameter::random_seed = atoi(argv[++i]);
			else if (!option.compare("-simmode")) {
				string sim_mode = argv[++i];
				if (!sim_mode.compare("cycle")) {
					CommonParameter::sim_mode = SIM_MODE_CYCLE;
				} else if (!sim_mode.compare("packet")) {
					CommonParameter::sim_mode = SIM_MODE_PACKET;
				} else {
					cout << "ERROR: Invalid simulation mode!" << endl;
					cout << "Please choose one mode: cycle or packet." << endl;
					exit(0);
				}
			} else if (!option.compare("-sim")) {
				if (CommonParameter::sim_mode == SIM_MODE_CYCLE)
					CommonParameter::simulation_time = atoi(argv[++i]);
				else
					CommonParameter::max_n_rx_packets = atoi(argv[++i]);
			} else if (!option.compare("-warm"))
				CommonParameter::warmup_time = atoi(argv[++i]);
			else if (!option.compare("-vcd")) {
				CommonParameter::is_vcd_traced = true;
				CommonParameter::vcd_filename = argv[++i];
			} else if (!option.compare("-platform")) {
				string platform_type = argv[++i];
				if (!platform_type.compare("synthetic"))
					CommonParameter::platform_type = PLATFORM_SYNTHETIC;
				else if (!platform_type.compare("embedded"))
					CommonParameter::platform_type = PLATFORM_EMBEDDED;
				else if (!platform_type.compare("dc"))
					CommonParameter::platform_type = PLATFORM_DC;
				else {
					cout << "ERROR: Invalid platform!" << endl;
					exit(0);
				}
			} else if (!option.compare("-delivery")) {
				string delivery_type = argv[++i];
				if (!delivery_type.compare("ack"))
					ProcessorParameters::packet_delivery_type =
					DELIVERY_WITH_ACK;
				else
					ProcessorParameters::packet_delivery_type =
					DELIVERY_WITHOUT_ACK;
			} else if (!option.compare("-ackpolicy")) {
				string ack_policy = argv[++i];
				if (!ack_policy.compare("always"))
					ProcessorParameters::sending_ack_type = SENDING_ACK_ALWAYS;
				else if (!ack_policy.compare("optimal"))
					ProcessorParameters::sending_ack_type = SENDING_ACK_OPTIMAL;
				else {
					cout << "ERROR: ACK sending policy is invalid!" << endl;
					exit(0);
				}
			} else if (!option.compare("-mapping")) {
				string mapping = argv[++i];
				if (!mapping.compare("random"))
					EmbeddedParameters::mapping_algorithm = MAPPING_RANDOM;
				else if (!mapping.compare("nmap"))
					EmbeddedParameters::mapping_algorithm = MAPPING_NMAP;
				else if (mapping == "KhalidDC" || mapping == "MinComm"
						|| mapping == "Static" || mapping == "StaticSM"
						|| mapping == "ZigZag" || mapping == "ZigZagSM"
						|| mapping == "3Core") {
					DCParameters::mapping = mapping;
					if (!mapping.compare("Static")) {
						DCParameters::mappingFile = argv[++i];
					}
				} else {
					cout << "ERROR: Task mapping algorithm is invalid!" << endl;
					exit(-1);
				}
			} else if (!option.compare("-appfile")) {
				EmbeddedParameters::app_filename = argv[++i];
			} else if (!option.compare("-app")) {
				DCParameters::appXml = argv[++i];
			} else if (!option.compare("-scheduling")) {
				DCParameters::scheduling = argv[++i];
			} else if (!option.compare("-outputFolder")) {
				DCParameters::outputFolder = argv[++i];
				struct stat info;
				if (stat(DCParameters::outputFolder.c_str(), &info) != 0) {
					std::cerr << "cannot access output folder "
							<< DCParameters::outputFolder << std::endl;
					exit(-1);
				} else if (!(info.st_mode & S_IFDIR)) {
					std::cerr << "output folder " << DCParameters::outputFolder
							<< " is not a folder but a file" << std::endl;
					exit(-1);
				}
			} else if (!option.compare("-np")) {
				DCParameters::dontHandlePeriodic = true;
			} else if (!option.compare("-freq")) {
				DCParameters::coresFrequencyInHz = atol(argv[++i]);
				if (DCParameters::coresFrequencyInHz > 1E9) {
					std::cerr << "Maximum supported frequency is 1GHz"
							<< std::endl;
					exit(-1);
				}
			} else if (!option.compare("-ierations")) {
				DCParameters::iterations = atoi(argv[++i]);
			} else if (!option.compare("-d")) {
				DCParameters::seqDep = true;
			} else if (!option.compare("-mw")) {
				DCParameters::useMicroworkload = true;
			} else if (!option.compare("-mww")) {
				DCParameters::microworkloadWidth = atoi(argv[++i]);
			}  else if (!option.compare("-mwh")) {
				DCParameters::microworkloadHeight = atoi(argv[++i]);
			} else {
				cerr << "ERROR: invalid command line inputs!\n"
						<< "No option name \'" << option << "\'" << endl;
				exit(-1);
			}
		}
	}
}

void show_help() {
	cout
			<< "   NoCTweak: a Highly Parameterizable On-Chip Network Simulator, version 1.0"
			<< endl;
	cout << "   Authours:   Anh Tran\n" << "               Bevan Baas\n"
			<< "              (at the VCL Lab, ECE Department, UC Davis)"
			<< endl;
	cout << "   Inputs:" << endl;
	cout << "      -help:	shows the option list\n\n";
	cout
			<< "      ------------------- Platform Options -----------------------------\n";
	cout
			<< "      -platform [option]:	application traffic simulated on this platform.\n"
			<< "         option = synthetic:	a synthetic traffic pattern (default)\n"
			<< "         option = embedded:	an embedded application trace\n"
			<< "      -seed [value]:	random seed for the simulation.\n"
			<< "            (the same random seed will drive the same output results for\n"
			<< "            the same network configuration. It's used for easier debugging.\n"
			<< "             Default value = system time.)\n"
			<< "      -log [filename]:	log file for simulation outputs\n"
			<< "      -vcd [filename]:	VCD file for signal waveform traces\n"
			<< "      -simmode [option]:	simulation mode (packet or cycle)\n"
			<< "      -simtime [value]:	simulation running time\n"
			<< "         value = N. Default = 100,000.\n"
			<< "         if simmode option = packet:	stop simulation after transferring N packets\n"
			<< "         if simmode option = cycle:	stop simulation after running N clock cycles\n"
			<< "      -warmtime [value]:	warmup time for the network to become stable\n"
			<< "         value = M (M < N). Default = 10,000.\n"
			<< "         if simmode option = packet:	do not consider the first M received packets\n"
			<< "         if simmode option = cycle:	warmup time is M clock cycles\n\n";

	cout
			<< "      ------------------ Synthetic Options -----------------------------\n";
	cout
			<< "      -dimx [value]:	X dimension length of the 2-D mesh network. Default value = 8.\n"
			<< "      -dimy [value]:	Y dimension length of the 2-D mesh network. Default value = 8.\n"
			<< "      -traffic [option]:	synthetic traffic patterns used for the simulation.\n"
			<< "         option = random:		uniform random (default)\n"
			<< "         option = transpose:	transpose\n"
			<< "         option = bitc:		bit-complement\n"
			<< "         option = bitr:		bit-reverse\n"
			<< "         option = tornado:		tornado\n"
			<< "         option = shuffle:		bit-shuffle\n"
			<< "         option = rotate:		bit-rotate\n"
			<< "         option = neighbor:	nearest neighbor traffic\n"
			<< "         option = regional:	communication distance <= 3\n"
			<< "         option = hotspot:		central or corner hot spots\n"
			<< "            -nhs [value]:		the number of hot spots. Default = 4.\n"
			<< "            -hstype [option]:	hot-spot type\n"
			<< "         option = central:		hot spots	at the central cores\n"
			<< "         option = corner:		hot spots at the corners (default)\n"
			<< "            -percent [value]:	percentage of traffics going to neighboring or regional or hotspot cores\n\n";

	cout
			<< "      ------------------ Embedded Application Traces ----------------------\n";
	cout
			<< "      -appfile [option]:	application task communication graph used in the simulation.\n"
			<< "         option = vopd.app:		video object plan decoder with 16 tasks (default)\n"
			<< "         option = mms.app:			multimedia system with 25 tasks\n"
			<< "         option = mwd.app:			multi-window display with 12 tasks\n"
			<< "         option = wifirx.app:		WiFi baseband receiver with 25 tasks\n"
			<< "         option = cavlc.app:		H.24 CAVLC encoder with 16 tasks\n"
			<< "         option = mpeg4.app:		MPEG4 decoder with 12 tasks\n"
			<< "         option = vce.app:			video conference encoder with 25 tasks\n"
			<< "         option = autoindust.app:	E3S auto-indust benchmark with 24 tasks\n"
			<< "         option = consumer.app:	E3S consumer benchmark with 12 tasks\n"
			<< "         option = telecom.app:		E3S telecom benchmark with 30 tasks\n"
			<< "      -mapping [option]:	mapping algorithm used to map the task graph to the processor array\n"
			<< "         option = random:	random mapping\n"
			<< "         option = nmap:	near-optimal mapping using the NMAP algorithm\n\n";

	cout
			<< "      ------------------- Traffic Options --------------------------------\n";
	cout
			<< "      -fir [value]:	flit injection rate (number of flits injected by each core per cycle)\n"
			<< "         0 < fir <= 1. Default = 0.2\n"
			<< "      -dist [option]:	probability distribution of the period between two injected packets\n"
			<< "         option = exponential:	exponential distribution (default)\n"
			<< "         option = identical:	identical distribution\n"
			<< "      -plengthtype [option]:	packet length is fixed or variable\n"
			<< "         option = fixed:		fixed packet length (default)\n"
			<< "         option = variable:	variable packet length\n"
			<< "      -plength [value]:	the number of flits per packet.\n"
			<< "         (only for the fixed packet length option. Default = 5.)\n"
			<< "      -plengthmin [value]:	the minimum number of flits per packet\n"
			<< "         (only for the variable packet length option. Default = 2.)\n"
			<< "      -plengthmax [value]:	the maximum number of flits per packet\n"
			<< "         (only for the variable packet length option. Default = 10.)\n\n";

	cout
			<< "      ------------------- Router Settings --------------------------------\n";
	cout << "      -router [option]    the simulated router\n"
			<< "         option = wh:			wormhole router (default)\n"
			<< "         option = vc:			virtual-channel router\n"
			<< "         option = roshaq:		RoShaQ share-queues router\n"
			<< "         option = bufferless:	bufferless router\n"
			<< "         option = cs:			circuit-switched router\n"
			<< "      -pptype [value]:		pipeline type and the number of pipeline stages. Default = 3 stages\n"
			<< "      -bsize [value]:		buffer depth (2, 4, 8, 16, 32 flits). Default = 4 flits.\n"
			<< "      -sbsize [value]:		shared-buffer queue depth (2, 4, 8, 16, 32 flits). Default = 4 flits.\n"
			<< "      -nvc [value]:		the number of virtual-channel buffers per input port. Default = 2 queues.\n"
			<< "      -nsb [value]:		the number of shared-buffer queues in RoShaQ routers. Default = 5 queues.\n"
			<< "      -routing [option]:	routing algorithm\n"
			<< "         option = xy:		XY dimension-ordered routing (default)\n"
			<< "         option = nfminimal:	Negative-First minimal adaptive routing\n"
			<< "         option = wfminimal:	West-First minimal adaptive routing\n"
			<< "         option = nlminimal:	North-Last minimal adaptive routing\n"
			<< "         option = oeminimal:	Odd-Even minimal adaptive routing\n"
			<< "         option = table:		lookup table based routing\n"
			<< "      -outsel [option]:	choose an output port among multiple ones returned by an adaptive routing\n"
			<< "         option = xyordered:	the X dimension first (default)\n"
			<< "         option = nearestdim:	the dimension nearest to the destination first\n"
			<< "         option = farthestdim:	the dimension farthest to the destination	first\n"
			<< "         option = roundrobin:	round-robin among output ports\n"
			<< "         option = credit:		the output port having the highest credit first\n"
			<< "      -sa [option]:	switch arbitration policy\n"
			<< "         option = rr:		round-robin (default)\n"
			<< "         option = oldest:	oldest first\n"
			<< "         option = takeall:	winner takes all (only for virtual-channel routers)\n"
			<< "         option = islip:	iSLIP based algorithm (only for virtual-channel routers)\n"
			<< "      -vca [option]:		virtual-channel allocation policy (only for virtual-channel routers)\n"
			<< "         option = rr:		round-robin (default)\n"
			<< "         option = oldest:	oldest first\n"
			<< "         option = islip:	iSLIP based algorithm\n"
			<< "      -llength [value]:	inter-router link length (in um). Default = 1000 um.\n\n";

	cout
			<< "      --------------- Environment Settings ------------------------ \n";
	cout
			<< "      -technode [value]:	CMOS technology process (90, 65, 45, 32, 22 nm). Default = 65 nm.\n"
			<< "      -freqmode [option]:	clock frequency setting\n"
			<< "         option = fixed:	fixed clock frequency (in MHz)\n"
			<< "         option = max:		the maximum clock frequency supported by the router\n"
			<< "      -freq [value]:	for fixed clock frequency (in MHz). Default = 1000 MHz.\n"
			<< "      -volt [value]:	supply voltage (in V). Default = 1.0 V.\n";

}

/*
 * processor settings
 */
void show_processor_settings() {
	string delivery_type;
	switch (ProcessorParameters::packet_delivery_type) {
	case DELIVERY_WITH_ACK:
		delivery_type = "WITH ACK";
		break;
	case DELIVERY_WITHOUT_ACK:
		delivery_type = "WITHOUT ACK";
		break;
	default:
		;
	}
	cout << "Packet delivery type: " << delivery_type << endl;

	string sending_ack_type;
	switch (ProcessorParameters::sending_ack_type) {
	case SENDING_ACK_ALWAYS:
		sending_ack_type = "ALWAYS SEND ACK";
		break;
	case SENDING_ACK_OPTIMAL:
		sending_ack_type = "SEND ACK OPTIMALLY";
		break;
	default:
		;
	}
	cout << "Sending ACK policy: " << sending_ack_type << endl;

	string dist_type;
	switch (ProcessorParameters::inter_arrival_time_type) {
	case (INTER_ARRIVAL_IDENTICAL):
		dist_type = "IDENTICAL";
		break;
	case (INTER_ARRIVAL_EXPONENTIAL):
		dist_type = "EXPONENTIAL";
		break;
	default:
		;
	}
	cout << "Packet inter-arrival time distribution: " << dist_type << endl;

	if (ProcessorParameters::packet_length_type == PACKET_LENGTH_TYPE_FIXED)
		cout << "Fixed packet length = " << ProcessorParameters::packet_length
				<< " (flits)" << endl;
	else {
		cout << "Variable packet length with Min = "
				<< ProcessorParameters::min_packet_length << " and Max = "
				<< ProcessorParameters::max_packet_length << " (flits)" << endl;
	}

	cout << "Flit injection rate = " << ProcessorParameters::flit_inject_rate
			<< " (flits/cycle/node)" << endl;

}

// show synthetic network settings
void show_synthetic_settings() {
	string traffic_type;
	switch (SyntheticParameters::traffic_type) {
	case (TRAFFIC_RANDOM):
		traffic_type = "UNIFORM-RANDOM";
		break;
	case (TRAFFIC_TRANSPOSE):
		traffic_type = "TRANSPOSE";
		break;
	case (TRAFFIC_BIT_COMPLEMENT):
		traffic_type = "BIT-COMPLEMENT";
		break;
	case (TRAFFIC_BIT_REVERSAL):
		traffic_type = "BIT-REVERSAL";
		break;
	case (TRAFFIC_TORNADO):
		traffic_type = "TORNADO";
		break;
	case (TRAFFIC_REGIONAL):
		traffic_type = "REGIONAL";
		break;
	case (TRAFFIC_NEIGHBOR):
		traffic_type = "NEIGHBOR";
		break;
	case (TRAFFIC_HOTSPOT):
		traffic_type = "HOTSPOT";
		break;
	case (TRAFFIC_SHUFFLE):
		traffic_type = "SHUFFLE";
		break;
	case (TRAFFIC_ROTATE):
		traffic_type = "ROTATE";
		break;

	default:
		;
	}
	cout << "Traffic type = " << traffic_type << endl;
//	log_file << "Traffic type = " << traffic_type << endl;

	if (SyntheticParameters::traffic_type == TRAFFIC_REGIONAL) {
		cout << "Regional distance = " << SyntheticParameters::regional_distance
				<< endl;
		cout << "Regional percentage = "
				<< SyntheticParameters::regional_percentage << "%" << endl;

//		log_file << "Regional distance = " << SyntheticParameters::regional_distance << endl;
//		log_file << "Regional percentage = " << SyntheticParameters::regional_percentage << "%" << endl;
	}

	if (SyntheticParameters::traffic_type == TRAFFIC_HOTSPOT) {
		cout << "Number of hotspots = " << SyntheticParameters::n_hotspots
				<< endl;
//		log_file << "Number of hotspots = " << SyntheticParameters::n_hotspots << endl;

		if (SyntheticParameters::hotspot_type == HOTSPOT_CENTRAL) {
			cout << "Hotspot type = CENTRAL" << endl;
//			log_file << "Hotspot type = CENTRAL" << endl;
		} else if (SyntheticParameters::hotspot_type == HOTSPOT_CORNER) {
			cout << "Hotspot type = CORNER" << endl;
//			log_file << "Hotspot type = CORNER" << endl;
		}

		cout << "Hotspot percentage = "
				<< SyntheticParameters::hotspot_percentage << "%" << endl;
//		log_file << "Hotspot percentage = " << SyntheticParameters::hotspot_percentage << "%" << endl;
	}

	if (SyntheticParameters::traffic_type == TRAFFIC_NEIGHBOR) {
		cout << "Neighbor percentage = "
				<< SyntheticParameters::neighbor_percentage << "%" << endl;
//		log_file << "Neighbor percentage = " << SyntheticParameters::neighbor_percentage << "%" << endl;
	}

}

/*
 * embedded settings
 */
void show_embedded_settings() {
	cout << "Embedded application file: " << EmbeddedParameters::app_filename
			<< endl;

	string mapping_algo;
	switch (EmbeddedParameters::mapping_algorithm) {
	case (MAPPING_RANDOM):
		mapping_algo = "RANDOM";
		break;
	case (MAPPING_NMAP):
		mapping_algo = "NMAP";
		break;
	default:
		;
	}
	cout << "Embedded task mapping algorithm: " << mapping_algo << endl;
}

/*
 * show_router_settings
 */
void show_router_settings() {
	string router_type;
	switch (RouterParameter::router_type) {
	case (ROUTER_WORMHOLE):
		router_type = "WORMHOLE";
		break;
	case (ROUTER_VC):
		router_type = "VIRTUAL-CHANNEL";
		break;
	case (ROUTER_SB_1IB):
		router_type = "SHARED-BUFFER with 1 Buffer per Input Port";
		break;
	case (ROUTER_LOWCOST_ONE_ENTRY):
		router_type = "LOW-COST with 1 Entry Buffer per Input Port";
		break;
	case (ROUTER_BUFFERLESS_EFC):
		router_type = "BUFFERLESS-EFC";
		break;
	case (ROUTER_WORMHOLE_PIPELINE):
		router_type = "WORMHOLE-PIPELINE";
		break;
	default:
		;
	}
	cout << "Router type: " << router_type << endl;

	if (RouterParameter::router_type == ROUTER_VC) {
		cout << "The number of VCs per port = " << RouterParameter::n_VCs
				<< endl;
	} else {
		cout << "NON VC router with " << RouterParameter::n_VCs
				<< " virtual channels " << endl;
	}

	if (RouterParameter::router_type == ROUTER_SB_1IB) {
		if (RouterParameter::n_VCs > 1) {
			cout
					<< "ERROR: the number of VCs per input port of ROUTER_SB_1IB is greater than 1!"
					<< endl;
			exit(0);
		} else {
			cout << "The number of SBs = " << RouterParameter::n_SBs << endl;
		}
	}

	string routing_type;
	switch (RouterParameter::routing_type) {
	case ROUTING_XY:
		routing_type = "XY DIMENSION-ORDERED";
		break;
	case ROUTING_SOURCE:
		routing_type = "SOURCE-ROUTING";
		break;
	case ROUTING_MINIMAL_WEST_FIRST:
		routing_type = "MINIMAL WEST-FIRST";
		break;
	case ROUTING_MINIMAL_NORTH_LAST:
		routing_type = "MINIMAL NORTH-LAST";
		break;
	case ROUTING_MINIMAL_NEGATIVE_FIRST:
		routing_type = "MINIMAL NEGATIVE-FIRST";
		break;
	case ROUTING_MINIMAL_ODD_EVEN:
		routing_type = "MINIMAL ODD-EVEN";
		break;
	default:
		;
	}
	cout << "Routing algorithm = " << routing_type << endl;

	string output_sel_policy;
	switch (RouterParameter::output_channel_select) {
	case OUTPUT_CHANNEL_SELECT_XY_ORDERED:
		output_sel_policy = "XY-ORDERED";
		break;
	case OUTPUT_CHANNEL_SELECT_NEARER_DIMENSION:
		output_sel_policy = "NEARER-DIMENSION";
		break;
	case OUTPUT_CHANNEL_SELECT_FARTHER_DIMENSION:
		output_sel_policy = "FARTHER-DIMENSION";
		break;
	case OUTPUT_CHANNEL_SELECT_ROUNDROBIN:
		output_sel_policy = "ROUND-ROBIN";
		break;
	case OUTPUT_CHANNEL_SELECT_HIGHER_CREDIT:
		output_sel_policy = "HIGHER-CREDIT";
		break;
	default:
		;
	}
	cout << "Output channel selection: " << output_sel_policy << endl;

	cout << "Packet length awareness: " << RouterParameter::packet_length_aware
			<< endl;

	string vca_type;
	switch (RouterParameter::vca_type) {
	case (VCA_ROUNDROBIN):
		vca_type = "ROUND_ROBIN";
		break;
	case (VCA_iSLIP):
		vca_type = "iSLIP";
		break;
	default:
		;
	}

	if ((RouterParameter::router_type == ROUTER_VC)
			|| (RouterParameter::router_type == ROUTER_EVC))
		cout << "VCA algorithm: " << vca_type << endl;

	string sa_type;
	switch (RouterParameter::sa_type) {
	case (SA_ROUNDROBIN):
		sa_type = "ROUND_ROBIN";
		break;
	case (SA_iSLIP):
		sa_type = "iSLIP";
		break;
	case (SA_WINNER_TAKE_ALL):
		sa_type = "WINNER_TAKE_ALL";
		break;

	default:
		;
	}

	if ((RouterParameter::router_type == ROUTER_VC)
			|| (RouterParameter::router_type == ROUTER_EVC))
		cout << "SA algorithm: " << sa_type << endl;

	string sba_type;
	switch (RouterParameter::sba_type) {
	case (SBA_ROUNDROBIN):
		sba_type = "ROUND_ROBIN";
		break;
	case (SBA_iSLIP):
		sba_type = "iSLIP";
		break;
	default:
		;
	}

	if (RouterParameter::router_type == ROUTER_SB_1IB) {
		cout << "SBA algorithms: " << sba_type << endl;
		cout << "SBA threshold: " << RouterParameter::sba_threshold << endl;
		cout << "SBA regulated: " << (bool) RouterParameter::sba_regulated
				<< endl;
	}

	cout << "Buffer size = " << RouterParameter::buffer_size << " (flits)"
			<< endl;

	if (RouterParameter::router_type == ROUTER_SB_1IB)
		cout << "Shared-Buffer size = " << RouterParameter::shared_buffer_size
				<< " (flits)" << endl;

	cout << "Inter-router link length = " << RouterParameter::link_length
			<< " (um)" << endl;

	cout << "Pipeline type = " << RouterParameter::pipeline_type << endl;
	cout << "Pipeline stages = " << RouterParameter::n_pipeline_stages << endl;

	cout << "Input voltage = " << CommonParameter::input_voltage << " (V)"
			<< endl;
	cout << "Input clock frequency = " << CommonParameter::input_clk_freq
			<< " (MHz)" << endl;
	cout << "Operating clock frequency = "
			<< CommonParameter::operating_clk_freq << " (MHz)" << endl;
}
