/*
 * tile.h
 *
 *  Created on: May 2, 2010
 *      Author: Anh Tran
 *
 *  Tile = Proc + Router
 *
 *  Revised:
 *  	+ 2010.11.18:
 *  		- support credit-based flow control
 */

#ifndef TILE_H_
#define TILE_H_

#include "proc/virtual_proc.h"
#include "proc/synthetic/without_ACK/synthetic_proc.h"
#include "proc/synthetic/with_ACK/synthetic_with_ACK_proc.h"
#include "proc/embedded/without_ACK/embedded_proc.h"
#include "proc/embedded/with_ACK/embedded_with_ACK_proc.h"
#include "proc/dreamcloud/dc_parameters.h"
#include "proc/dreamcloud/dc_proc.h"
#include "proc/dreamcloud/dc_proc_type.hxx"
#include "router/vcr_rt_credit/vcr_rt_credit_router.h"
#include "router/vcr_credit/vcr_credit_router.h"
#include "router/virtual_router.h"
#include "router/wormhole_pipeline/wormhole_pipeline.h"

SC_MODULE (Tile) {

	// local address
	int local_x;
	int local_y;

	sc_in<bool> clk;
	sc_in<bool> reset;

	// input ports
	sc_in<bool> valid_in[ROUTER_PORTS - 1];
	sc_in<Flit> flit_in[ROUTER_PORTS - 1];
//	sc_out <bool> in_full[ROUTER_PORTS-1][MAX_N_VCS];
	sc_out<bool> in_vc_buffer_rd[ROUTER_PORTS - 1][MAX_N_VCS];

	// output ports
	sc_out<bool> valid_out[ROUTER_PORTS - 1];
	sc_out<Flit> flit_out[ROUTER_PORTS - 1];
//	sc_in <bool> out_full[ROUTER_PORTS-1][MAX_N_VCS];
	sc_in<bool> out_vc_buffer_rd[ROUTER_PORTS - 1][MAX_N_VCS];

	// signals and variables
	sc_signal<bool> tx_valid_out;
	sc_signal<Flit> tx_flit_out;
//	sc_signal <bool> tx_out_full[MAX_N_VCS];
	sc_signal<bool> tx_out_vc_buffer_rd[MAX_N_VCS];

	sc_signal<bool> rx_valid_in;
	sc_signal<Flit> rx_flit_in;
//	sc_signal <bool> rx_in_full[MAX_N_VCS];
	sc_signal<bool> rx_in_vc_buffer_rd[MAX_N_VCS]; // only consider buffer_rd[0]; is processor read an input flit?

	// setup don't-care signals
	void initialize() {
	}

	// modules
	VirtualProc *proc;
	VirtualRouter *router;

	SC_HAS_PROCESS(Tile);
	Tile(sc_module_name name, int x, int y) :
			local_x(x), local_y(y) {
		switch (CommonParameter::platform_type) {
		case (PLATFORM_SYNTHETIC): {
			switch (ProcessorParameters::packet_delivery_type) {
			case DELIVERY_WITHOUT_ACK:
				proc = new SyntheticProc("Synthetic_Proc");
				break;
			case DELIVERY_WITH_ACK:
				proc = new SyntheticWithACKProc("Synthetic_With_ACK_Proc");
				break;
			default:
				;
			}
			break;
		}
		case (PLATFORM_EMBEDDED): {
			switch (ProcessorParameters::packet_delivery_type) {
			case DELIVERY_WITHOUT_ACK:
				proc = new EmbeddedProc("Embedded_Proc");
				break;
			case DELIVERY_WITH_ACK:
				proc = new EmbeddedWithACKProc("Synthetic_With_ACK_Proc");
				break;
			default:
				;
			}
			break;
		}
		case (PLATFORM_DC): {
			processingElementType *type = new processingElementType(
					DCParameters::coresFrequencyInHz,
					DCParameters::instructionsPerCycle);
			proc = new DcProc("DCProc", type);
			break;
		}
		default: {
			cout << "ERROR: Invalid Platform Type!" << endl;
			exit(0);
		}
		}
		proc->clk(clk);
		proc->reset(reset);
		proc->valid_out(tx_valid_out);
		proc->flit_out(tx_flit_out);
		for (int k = 0; k < MAX_N_VCS; k++) {
			proc->out_vc_buffer_rd[k](tx_out_vc_buffer_rd[k]);
		}
		proc->valid_in(rx_valid_in);
		proc->flit_in(rx_flit_in);
		for (int k = 0; k < MAX_N_VCS; k++) {
			proc->in_vc_buffer_rd[k](rx_in_vc_buffer_rd[k]);
		}

		switch (RouterParameter::router_type) {
		case (ROUTER_WORMHOLE):
//				cout << "Binding Wormhole Router ..." << endl;
//				router = new WormholeRouter("Wormhole_Router");
			/*
			 WormholeRouter *wh_router = (WormholeRouter*) router;

			 wh_router->clk(clk);
			 wh_router->reset(reset);
			 wh_router->valid_in[LOCAL](tx_valid_out);
			 wh_router->flit_in[LOCAL](tx_flit_out);
			 wh_router->in_full[LOCAL](tx_out_full[0]);

			 wh_router->valid_out[LOCAL](rx_valid_in);
			 wh_router->flit_out[LOCAL](rx_flit_in);
			 wh_router->out_full[LOCAL](rx_in_full[0]);

			 for (int i=0; i<ROUTER_PORTS-1; i++){
			 wh_router->valid_in[i](valid_in[i]);
			 wh_router->flit_in[i](flit_in[i]);
			 wh_router->in_full[i](in_full[i][0]);

			 wh_router->valid_out[i](valid_out[i]);
			 wh_router->flit_out[i](flit_out[i]);
			 wh_router->out_full[i](out_full[i][0]);
			 }
			 */
			break;
		case (ROUTER_VC):

			switch (RouterParameter::n_VCs) {
			case (1):
				router = new VCRouter<1>("Virtual_Channel_Router_1_VC");
				break;
			case (2):
				router = new VCRouter<2>("Virtual_Channel_Router_2_VCs");
				break;
			case (4):
				router = new VCRouter<4>("Virtual_Channel_Router_4_VCs");
				break;
			case (8):
				router = new VCRouter<8>("Virtual_Channel_Router_8_VCs");
				break;
			}
			break;
		case (ROUTER_VC_RT): {
			string name = "Virtual_Channel_RT_Router_"
					+ int_to_str(RouterParameter::n_VCs) + "_VCs_Tile["
					+ int_to_str(local_x) + "][" + int_to_str(local_y) + "]";
			switch (RouterParameter::n_VCs) {
			case (1):
				router = new VCRTRouter<1>(name.c_str());
				break;
			case (2):
				router = new VCRTRouter<2>(name.c_str());
				break;
			case (4):
				router = new VCRTRouter<4>(name.c_str());
				break;
			case (8):
				router = new VCRTRouter<8>(name.c_str());
				break;
			case (16):
				router = new VCRTRouter<16>(name.c_str());
				break;
			default:
				cerr << "Unsupported number of virtual channels: "
						<< RouterParameter::n_VCs << endl;
				exit(-1);
			}
		}
		case (ROUTER_SB_1IB):
			break;
		case (ROUTER_LOWCOST_ONE_ENTRY):
			break;
		case (ROUTER_WORMHOLE_PIPELINE):
			router = new WormholePipeline("Wormhole_Pipeline");
			break;
		default:
			cout << "ERROR: Invalid Router Type!" << endl;
			exit(0);
			break;
		}

		router->clk(clk);
		router->reset(reset);

		router->valid_in[LOCAL](tx_valid_out);
		router->flit_in[LOCAL](tx_flit_out);
		for (int k = 0; k < MAX_N_VCS; k++) {
			router->in_vc_buffer_rd[LOCAL][k](tx_out_vc_buffer_rd[k]);
		}

		router->valid_out[LOCAL](rx_valid_in);
		router->flit_out[LOCAL](rx_flit_in);
//		router->out_full[LOCAL][0](rx_in_full);
		for (int k = 0; k < MAX_N_VCS; k++) {
			router->out_vc_buffer_rd[LOCAL][k](rx_in_vc_buffer_rd[k]);
		}

		for (int i = 0; i < ROUTER_PORTS - 1; i++) {
			router->valid_in[i](valid_in[i]);
			router->flit_in[i](flit_in[i]);
			for (int k = 0; k < MAX_N_VCS; k++) {
				router->in_vc_buffer_rd[i][k](in_vc_buffer_rd[i][k]);
			}

			router->valid_out[i](valid_out[i]);
			router->flit_out[i](flit_out[i]);
			for (int k = 0; k < MAX_N_VCS; k++) {
				router->out_vc_buffer_rd[i][k](out_vc_buffer_rd[i][k]);
			}
		}

	}
};

#endif /* TILE_H_ */
