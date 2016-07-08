/*
 * crossbar.h
 *
 * (C) copyright by VCL, UC Davis
 * required to send an email to anhtrandavis@gmail.com before using this code;
 * and cite the paper "..." in your related publications
 *
 *  Created on: Apr 25, 2010
 *      Author: Anh Tran
 *
 *  Model of a NxN crossbar
 */

#ifndef VC_RT_CROSSBAR_H_
#define VC_RT_CROSSBAR_H_

#include "systemc.h"
#include "../../definition.h"
#include "../../MemoryProfiled.h"

template<int N_VCs>
#ifdef MEM_PROF
class VCRTCrossbar: public sc_module, MemoryProfiled<VCRTCrossbar<N_VCs> > {
#else
class VCRTCrossbar: public sc_module {
#endif

public:
//template <int N_VCs>
//SC_MODULE (VCRTCrossbar){
//	sc_in <bool> reset;

	sc_in <Flit> crossbar_in[ROUTER_PORTS];
	sc_in <bool> crossbar_in_valid[N_ROUTER_PORTS];

	sc_in <int> crossbar_grant[ROUTER_PORTS];	// pi


	sc_out <Flit> crossbar_out[ROUTER_PORTS];
	sc_out <bool> crossbar_out_valid[ROUTER_PORTS];

	// process
	void crossbar_process();

	SC_CTOR (VCRTCrossbar){
		SC_METHOD (crossbar_process);
		for (int pi=0; pi<N_ROUTER_PORTS; pi++){
			sensitive << crossbar_in[pi];
			sensitive << crossbar_in_valid[pi];
		}
		for (int po=0; po<N_ROUTER_PORTS; po++){
			sensitive << crossbar_grant[po];
		}
	}
};

#endif /* VC_CROSSBAR_H_ */

//================ inplemenataion

/*
 * crossbar process
 */
template <int N_VCs>
void VCRTCrossbar<N_VCs>::crossbar_process(){
	for (int po=0; po<N_ROUTER_PORTS; po++){
		if (crossbar_grant[po].read() >= 0){	// output port [po] was allocated
//			int indexi=crossbar_grant[po].read();
			int pi = crossbar_grant[po].read();

			crossbar_out_valid[po].write(crossbar_in_valid[pi].read());
			crossbar_out[po].write(crossbar_in[pi].read());
		}
		else {
			crossbar_out_valid[po].write(0);
			crossbar_out[po].write(Flit());
		}
	}
}
