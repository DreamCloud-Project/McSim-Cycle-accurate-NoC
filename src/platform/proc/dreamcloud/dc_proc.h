#ifndef DC_PROC_H_
#define DC_PROC_H_

#include <random>
#include <queue>
#include <systemc.h>
#include <vector>
#include "../virtual_proc.h"
#include "commons/parser/dcRunnableInstance.h"
#include "dc_proc_type.hxx"

using namespace std;
using namespace DCApplication;

class DcProc: public VirtualProc {

public:

	// Interface to receive runnables from mapper and notify it
	dcRunnableInstance *newRunnable;
	bool newRunnableSignal = false;
	sc_event *newRunnable_event;
	sc_event *runnableCompleted_event;

	// Statistics
	int injected_packet_count;
	int received_packet_count;

	// Scheduling strategy
	enum SchedulingStrategy {
		FCFS, PRIO
	};
	SchedulingStrategy sched;

	int deadlinesMissed = 0;
	vector<dcRunnableInstance *> completedRunnableInstances;

	// functions
	void initialize(int x, int y);

	// for evaluation
	ProcEvaluationFactors *evaluation();

	// Output files
	ofstream *nocTrafficCsvFile;
	ofstream *runnablesCsvFile;
	ofstream *instsCsvFile;

	// List of priorities in application to choose VC
	vector<int> priorities;

	// constructor with process enable
	SC_HAS_PROCESS(DcProc);
	DcProc(sc_module_name name, processingElementType *type_) :
			VirtualProc(name), injected_packet_count(0), received_packet_count(
					0), local_x(-1), local_y(-1), type(type_) {

		// receive flit
		SC_METHOD(flitIn_method);
		sensitive << reset.pos() << clk.pos();

		// flit_out
		SC_METHOD(flitOut_method);
		sensitive << reset.pos() << clk.pos();

		// Credit information coming back from router
		//SC_METHOD(locRouterBuffers_method);
//		for (int vo = 0; vo < RouterParameter::n_VCs; vo++) {
//			sensitive << out_vc_buffer_rd[vo];
//		}

		// Runnables executer
		SC_THREAD(runnableExecuter_thread);

		// Inits the generator for deviation instructions
		gen = std::mt19937(CommonParameter::random_seed);

		// Inits the number of free flit enties in the local input of the router associated to this PE
		for (int vi = 0; vi < RouterParameter::n_VCs; vi++) {
			out_vc_remain[vi] = RouterParameter::buffer_size;
		}

		// update out_vc_remain
		SC_METHOD(out_vc_remain_process);
		for (int vo = 0; vo < RouterParameter::n_VCs; vo++) {
			sensitive << out_vc_remain_reg[vo];
			sensitive << count_plus[vo];
			//sensitive << out_vc_buffer_rd[vo];
			sensitive << count_minus[vo];
		}

		// pipelined out_vc_remain
		SC_METHOD(out_vc_remain_reg_process);
		sensitive << clk.pos() << reset.pos();

		// count_plus = out_vc_buffer
		SC_METHOD(count_plus_process);
		for (int vo = 0; vo < RouterParameter::n_VCs; vo++) {
			sensitive << out_vc_buffer_rd[vo];
//			sensitive << out_port_EFC_en;
		}
	}

private:

	// processes
	void flitOut_method();
	void flitIn_method();
	void runnableExecuter_thread();
	void locRouterBuffers_method();
	void out_vc_remain_process();	// update out_vc_remain
	void count_plus_process();	// pipelined out_vc_remain
	void out_vc_remain_reg_process();	// pipelined out_vc_remain

	// local address
	int local_x;
	int local_y;

	static int nbRunBlocked;
	static int packetId;
	static int writeRequestId;
	static int readRequestId;

	// An infinite queue of injected flits
	queue<Flit> source_queue;

	// Keep trace of number of idle entries of each input VC of the local router
	int out_vc_remain[MAX_N_VCs];
	sc_signal<int> out_vc_remain_reg[MAX_N_VCs];
	sc_signal<bool> count_plus[MAX_N_VCs];	// = out_vc_buffer_rd
	sc_signal<bool> count_minus[MAX_N_VCs];

	// Keep track of the last head flit received on each output VC of the local router
	Flit lastHead[MAX_N_VCs];

	// Type definitions for managing runnables preemption
	struct runnableExecElement {
		int instructionIdx;
		dcRunnableInstance *runInstance;
		int priority;
	};
	struct runnableBlockedOnRemoteRead {
		 int readRequestId;
		 runnableExecElement execElem;
		 int xReplier;
		 int yReplier;
		 int nbPktReceived;
		 int nbPktToBeReceived;
	};

	// Management of the execution of runnables on this PE
	vector<runnableExecElement> readyRunnables;
	vector<runnableBlockedOnRemoteRead> runnablesBlockedOnRemoteRead;

	// Random number generator for deviation instructions
	std::mt19937 gen;

	// Proc type
	const processingElementType *type;

	// Internal functions for managing ready runnables
	void addReadyRunnable(runnableExecElement runnableExecElem);
	void removeReadyRunnable(dcRunnableInstance *runnable);
	void addBlockedRunnable(runnableBlockedOnRemoteRead blocked);

	// Functions to execute runnable items
	void executeInstructionsConstant(dcInstruction *inst,
			dcRunnableInstance *run, int instructionId);
	void executeInstructionsDeviation(dcInstruction *inst,
			dcRunnableInstance *run, int instructionId);
	void executeRemoteLabelWrite(dcRemoteAccessInstruction *rinst,
			dcRunnableInstance *run, int x, int y);
	void executeRemoteLabelRead(dcRemoteAccessInstruction *rinst,
			dcRunnableInstance *run, int x, int y, int instructionId, int priority);

	// Function handling packet reception
	void receivePkt(const Packet & p);
};

#endif /* DC_PROC_H_ */
