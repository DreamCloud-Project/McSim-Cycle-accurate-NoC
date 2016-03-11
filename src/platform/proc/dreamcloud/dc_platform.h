/*
 * noc.h
 *
 * (C) copyright by the VCL laboratory, ECE Department, UC Davis
 *
 *  Please send en email to anhtrandavis@gmail.com to ask for your permission
 *  before using this code;
 *  and cite the paper "..." in your related publications.
 *
 *
 *  Created on: Apr 22, 2010
 *      Author: Anh Tran
 *
 *  A 2-D Mesh NoC of Tiles (PE + Router)
 */

#ifndef DC_PLATFORM_H_
#define DC_PLATFORM_H_

#include <vector>
#include "parser/AmApplication.h"
#include "parser/dcTaskGraph.h"
#include "parser/dcApplication.h"
#include "mapping_heuristic/dcMappingHeuristicI.hxx"
#include "../../common_functions.h"
#include "../../tile.h"

using namespace std;
using namespace DCApplication;
using namespace dreamcloud::platform_sclib;

class DcPlatform: sc_module {

public:

	Tile *tile[MAX_DIM][MAX_DIM];

	sc_in<bool> clk;
	sc_in<bool> reset;

	sc_signal<bool> W_valid_in[MAX_DIM][MAX_DIM];
	sc_signal<Flit> W_flit_in[MAX_DIM][MAX_DIM];
	sc_signal<bool> W_in_vc_buffer_rd[MAX_DIM][MAX_DIM][MAX_N_VCS];

	sc_signal<bool> N_valid_in[MAX_DIM][MAX_DIM];
	sc_signal<Flit> N_flit_in[MAX_DIM][MAX_DIM];
	sc_signal<bool> N_in_vc_buffer_rd[MAX_DIM][MAX_DIM][MAX_N_VCS];

	sc_signal<bool> W_valid_out[MAX_DIM][MAX_DIM];
	sc_signal<Flit> W_flit_out[MAX_DIM][MAX_DIM];
	sc_signal<bool> W_out_vc_buffer_rd[MAX_DIM][MAX_DIM][MAX_N_VCS];

	sc_signal<bool> N_valid_out[MAX_DIM][MAX_DIM];
	sc_signal<Flit> N_flit_out[MAX_DIM][MAX_DIM];
	sc_signal<bool> N_out_vc_buffer_rd[MAX_DIM][MAX_DIM][MAX_N_VCS];

	SC_HAS_PROCESS(DcPlatform);
	DcPlatform(sc_module_name name);

private:

   // Amalthea application
	dcTaskGraph* taskGraph;
	dcApplication* application;
	vector<dcTask*> tasks;
	vector<dcLabel*> labels;

	// Management of runnables
	vector<dcRunnableInstance*> releasedRunnables;
	vector<dcRunnableCall*> runnables;
	void releaseRunnable(dcRunnableInstance *runnable);

	// Management of periodic runnables
	vector<dcRunnableCall*> periodicAndSporadicRunnables;
	unsigned long hyperPeriod;

	// The mapping heuristic modules
	dcMappingHeuristicI *mappingHeuristic;

	// SystemC processes and associated events
	void runnablesDispatcher_thread();
	sc_event runnableReleased_event;
	void labelsMapper_method();
	void dependentRunnablesReleaser_method();
	sc_event runnableCompleted_event;
	void periodicRunnablesReleaser_thread();
	void nonPeriodicIndependentRunnablesReleaser_method();
	sc_event iteration_event;
	void modeSwitcher_thread();
	void stopSimu_thread();
	sc_event stopSimu_event;

	// Events used to notify cores when new runnables are ready
	sc_vector<sc_vector<sc_event> > newRunnable_event;

	// Runnables counting
	unsigned int nbRunnablesCompleted;
	unsigned int nbRunnablesMapped;

	// Modes management
	typedef struct {
		unsigned long int time;
		string name;
		string file;
	} mode_t;
	vector<mode_t> modes;
	unsigned long int simulationEnd = 0;

	// Simulation start time
	clock_t start;

	// Output files
	ofstream nocTrafficCsvFile;
	ofstream runnablesCsvFile;
	ofstream instsCsvFile;

	// Utility functions
	void updateInstructionsExecTimeBounds(string newAppFile);
};

#endif /* DC_PLATFORM_H_ */
