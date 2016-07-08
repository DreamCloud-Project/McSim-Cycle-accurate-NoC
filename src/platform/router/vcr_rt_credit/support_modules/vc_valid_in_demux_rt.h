/*
 * vc_valid_in_demux.h
 *
 *  Created on: Nov 12, 2010
 *      Author: Anh Tran
 *
 *  Demuxing valid signals for each VC
 */

#ifndef VC_VALID_IN_DEMUX_RT_H_
#define VC_VALID_IN_DEMUX_RT_H_

#include <systemc.h>
#include "../../router_parameters.h"
#include "../../../MemoryProfiled.h"

#ifdef MEM_PROF
class VCValidInDemuxRT : public sc_module, MemoryProfiled<VCValidInDemuxRT> {
#else
class VCValidInDemuxRT : public sc_module {
#endif

public:
//SC_MODULE (VCValidInDemuxRT){
	int vc_id;	// vc_id of the input port

	sc_in <Flit> flit_in;
	sc_in <bool> valid_in;

	sc_out <bool> valid_out;

	void vc_valid_in_demux_process(){
		if (flit_in.read().vc_id == vc_id)
			valid_out.write(valid_in.read());
		else
			valid_out.write(0);
	}

	SC_CTOR (VCValidInDemuxRT){
		SC_METHOD (vc_valid_in_demux_process);
		sensitive << flit_in << valid_in;
	}
};

#endif /* VC_VALID_IN_DEMUX_H_ */
