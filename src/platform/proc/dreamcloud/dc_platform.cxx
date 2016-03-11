#include <ctime>
#include "dc_platform.h"
#include "commons/parser/dcAmaltheaParser.h"
#include "dc_parameters.h"
#include "commons/mapping_heuristic/dcMappingHeuristicKhalidDC.hxx"
#include "commons/mapping_heuristic/dcMappingHeuristicMinComm.hxx"
#include "commons/mapping_heuristic/dcMappingHeuristicStatic.hxx"
#include "commons/mapping_heuristic/dcMappingHeuristicWeka.hxx"
#include "commons/mapping_heuristic/dcMappingHeuristicStaticSM.hxx"
#include "commons/mapping_heuristic/dcMappingHeuristicZigZag.hxx"
#include "commons/mapping_heuristic/dcMappingHeuristicZigZagSM.hxx"
#include "commons/mapping_heuristic/dcMappingHeuristicZigZagThreeCore.hxx"
#include "commons/mapping_heuristic/dcMappingHeuristicRandom.hxx"
#include "commons/mapping_heuristic/uoyHeuristicModuleStatic.hxx"

DCLabelsMapping DCParameters::labelsMap;

DcPlatform::DcPlatform(sc_module_name name) :
		sc_module(name), newRunnable_event("newRunEvent",
				CommonParameter::dim_x) {

//	unsigned long size = 0;
//	unsigned long nbCores = CommonParameter::dim_x * CommonParameter::dim_y;

//	cerr << " Size of DcPlatform = " << sizeof(DcPlatform) << endl;
//	size = size + sizeof(DcPlatform);
//
//	cerr << " Size of DcProc = " << sizeof(DcProc) << endl;
//	size = size + sizeof(DcProc) * nbCores;
//
//	cerr << " Size of tile = " << sizeof(Tile) << endl;
//	size = size + sizeof(Tile) * nbCores;
//
//	cerr << " Size of vc_router = " << sizeof(VCRouter<8>) << endl;
//	size = size + sizeof(VCRouter<8>) * nbCores;
//
//	cerr << " Size of vc_buffer = " << sizeof(VCBuffer) << endl;
//	size = size + sizeof(VCBuffer) * nbCores * N_ROUTER_PORTS * RouterParameter::n_VCs;
//
//	cerr << " Size of vc_crossbar = " << sizeof(VCCrossbar<8>) << endl;
//	size = size + sizeof(VCCrossbar<8>)  * nbCores;
//
//	cerr << " Size of vc_allocator = " << sizeof(VCAllocator<8>) << endl;
//	size = size + sizeof(VCAllocator<8>) * nbCores;
//
//	cerr << " Size of sw_allocator = " << sizeof(SWAllocator<8>) << endl;
//	size = size + sizeof(SWAllocator<8>) * nbCores;
//
//	cerr << " Size of route_comp = " << sizeof(RouteCompVc) << endl;
//	size = size + sizeof(RouteCompVc) * nbCores;
//
//	cerr << " Size of in state update = " << sizeof(InVCStateUpdate) << endl;
//	size = size + sizeof(InVCStateUpdate) * nbCores * N_ROUTER_PORTS * RouterParameter::n_VCs;
//
//	cerr << " Size of out state update = " << sizeof(OutVCStateUpdate<8>) << endl;
//	size = size + sizeof(OutVCStateUpdate<8>) * nbCores * N_ROUTER_PORTS * RouterParameter::n_VCs;
//
//	cerr << " Size of sa stage 1 = " << sizeof(SAStage1_rt<8>) << endl;
//	size = size + sizeof(SAStage1_rt<8>) * nbCores * N_ROUTER_PORTS * RouterParameter::n_VCs;
//
//	cerr << " Size of sa stage 2 = " << sizeof(SAStage2_rt<8>) << endl;
//	size = size + sizeof(SAStage2_rt<8>) * nbCores * N_ROUTER_PORTS * RouterParameter::n_VCs;
//
//	cerr << " Size of vca stage 1 = " << sizeof(VCAStage1OutputVCChooserRT<8>) << endl;
//	size = size + sizeof(VCAStage1OutputVCChooserRT<8>) * nbCores * N_ROUTER_PORTS * RouterParameter::n_VCs;
//
//	cerr << " Size of vca stage 2 = " << sizeof(VCAStage2OutputVCArbitrer<8>) << endl;
//	size = size + sizeof(VCAStage2OutputVCArbitrer<8>) * nbCores * N_ROUTER_PORTS * RouterParameter::n_VCs;
//
//	cerr << "TOTAL size = " << size /1024.0 / 1024.0 << " MBytes" << endl;

	// Inits output files
	nocTrafficCsvFile.open(
			DCParameters::outputFolder + "/OUTPUT_NoC_Traces.csv");
	nocTrafficCsvFile << "Packet_ID,";
	nocTrafficCsvFile << "Read_Request_ID,";
	nocTrafficCsvFile << "Priority,";
	nocTrafficCsvFile << "Source,";
	nocTrafficCsvFile << "Destination,";
	nocTrafficCsvFile << "Injection_Time_(NS),";
	nocTrafficCsvFile << "Delivery_Time_(NS),";
	nocTrafficCsvFile << "Packet_Latency_(NS),";
	nocTrafficCsvFile << "Latency_No_Contention_(NS)";
	nocTrafficCsvFile << endl;
	runnablesCsvFile.open(
			DCParameters::outputFolder + "/OUTPUT_Runnable_Traces.csv");
	runnablesCsvFile << "Core_ID,";
	runnablesCsvFile << "Name,";
	runnablesCsvFile << "ID,";
	runnablesCsvFile << "Priority,";
	runnablesCsvFile << "Allocation_Time_(NS),";
	runnablesCsvFile << "Start_Time_(NS),";
	runnablesCsvFile << "Completion_Time_(NS),";
	runnablesCsvFile << "Duration_(NS),";
	runnablesCsvFile << "Deadline_(NS)";
	runnablesCsvFile << "Deadline_Met_Missed(NS)";
	runnablesCsvFile << endl;
	instsCsvFile.open(
				DCParameters::outputFolder + "/Executed_Computing_Instructions.csv");
	instsCsvFile << "Runnable_ID,";
	instsCsvFile << "Runnable_Name,";
	instsCsvFile << "Instruction_ID,";
	instsCsvFile << "Instruction_Count,";
	instsCsvFile << endl;

	// Init sc_vectors second dimension
	for (int i(0); i < CommonParameter::dim_x; ++i) {
		newRunnable_event.at(i).init(CommonParameter::dim_y);
	}

	// Check and get scheduling strategy
	DcProc::SchedulingStrategy sched;
	if (DCParameters::scheduling == "fcfs") {
		sched = DcProc::FCFS;
	} else if (DCParameters::scheduling == "prio") {
		sched = DcProc::PRIO;
	} else {
		cerr << "scheduling strategy unkownn " << endl;
		exit(-1);
	}

	// Start dreamcloud big brother if required
	if (CommonParameter::platform_type == PLATFORM_DC) {

		// Get app file or mode file
		AmApplication* amApplication = new AmApplication();
		string appPath = DCParameters::appXml;

		// Use the parser to create an AmApplication object
		dcAmaltheaParser amaltheaParser;
		amaltheaParser.ParseAmaltheaFile(appPath, amApplication);

		// Convert the AmApplication to a dcApplication
		int t0 = time(NULL);
		taskGraph = application->createGraph("dcTaskGraph");
		application = new dcApplication();
		application->CreateGraphEntities(taskGraph, amApplication,
				DCParameters::seqDep);
		tasks = application->GetAllTasks(taskGraph);
		labels = application->GetAllLabels(amApplication);
		runnables = application->GetAllRunnables(taskGraph);
		int t1 = time(NULL);
		application->GetAllPriorities(taskGraph);
		application->dumpRunnablesToFiles(taskGraph, DCParameters::outputFolder);

		// Assert we have the same instructionsPerCyle in
		// all core types in the app because today we are
		// only handling that case
		map<string, dcCoreType*> coreTypes = amApplication->getCoreTypesMap();
		typedef map<string, dcCoreType*>::iterator it_type;
		int instructionsPerCycle = 1;
		for (it_type iterator = coreTypes.begin(); iterator != coreTypes.end();
				iterator++) {
			if (instructionsPerCycle == 1) {
				instructionsPerCycle =
						iterator->second->GetInstructionsPerCycle();
			}
//			else {
//				if (iterator->second->GetInstructionsPerCycle()
//						!= instructionsPerCycle) {
//					cerr
//							<< "different instructionsPerCycle values found in application"
//							<< endl;
//					exit(-1);
//				}
//			}
		}
		DCParameters::instructionsPerCycle = instructionsPerCycle;

		// Create mapping heuristic module
		if (DCParameters::mapping == "MinComm") {
			dcMappingHeuristicMinComm *minComm =
					new dcMappingHeuristicMinComm();
			minComm->setRunnables(runnables);
			mappingHeuristic = minComm;
		} else if (DCParameters::mapping == "Static") {
			dcMappingHeuristicStatic *staticMapping =
					new dcMappingHeuristicStatic();
			staticMapping->setFile(DCParameters::mappingFile);
			mappingHeuristic = staticMapping;
		} else if (DCParameters::mapping == "StaticModes") {
			uoyHeuristicModuleStatic *staticMappingSM =
					new uoyHeuristicModuleStatic();
			staticMappingSM->ParseModeListFile(DCParameters::mappingFile);
			staticMappingSM->ParseModeChangeTiming(DCParameters::mappingFile);
			mappingHeuristic = staticMappingSM;
		} else if (DCParameters::mapping == "ZigZag") {
			mappingHeuristic = new dcMappingHeuristicZigZag();
		} else {
			cerr << "mapping heuristic unknown" << endl;
			exit(-1);
		}
		mappingHeuristic->setNocSize(CommonParameter::dim_x,
				CommonParameter::dim_y);
		mappingHeuristic->setApp(appPath);

		// Print information delivered by parser
		cout << " ##Application Information Delivered by Parser ##" << endl
				<< endl;
		periodicAndSporadicRunnables =
				application->GetPeriodicAndSporadicRunnables(taskGraph,
						DCParameters::seqDep);
		vector<dcRunnableCall*> independantNonPeriodicRunnables =
				application->GetIndependentNonPeriodicRunnables(taskGraph);
		hyperPeriod = application->GetHyperPeriodWithOffset(taskGraph);
		cout << "    Number of tasks: " << tasks.size() << endl;
		cout << "    Number of runnables: " << runnables.size() << endl;
		cout << "    Number of periodic and sporadic runnables: "
				<< periodicAndSporadicRunnables.size() << " (hyper period = "
				<< hyperPeriod << "ns)" << endl;
//		cout << "    Number of Initial Independant Runnables not Periodic: "
//				<< independantNonPeriodicRunnables.size() << endl;
//		cout << "    Number of Instructions: "
//				<< applicaton->GetAllInstructions(taskGraph).size() << endl;
		vector<int> prios = application->GetAllPriorities(taskGraph);
		cout << "    Number of different Priorities: " << prios.size() << " (";
		for (std::vector<int>::size_type i = 0; i != prios.size(); i++) {
			if (i > 0) {
				cout << ", ";
			}
			cout << prios[i];
		}
		cout << ")" << endl;

		cout << "    Number of labels: " << labels.size() << endl;
		cout << "    Parsing time: " << (t1 - t0) << " s" << endl << endl
				<< endl;

		// Dump graph files
		application->dumpTaskGraphFile(
				DCParameters::outputFolder + "/dcTasksGraphFile.gv", tasks);
		application->dumpTaskAndRunnableGraphFile(
				DCParameters::outputFolder + "/dcTasksAndRunsGraphFile.gv",
				tasks);

		// Create SystemC processes
		SC_METHOD(labelsMapper_method);
		SC_THREAD(runnablesDispatcher_thread);
		sensitive << runnableReleased_event;
		SC_THREAD(stopSimu_thread);
		sensitive << stopSimu_event;
		dont_initialize();
		SC_METHOD(dependentRunnablesReleaser_method);
		sensitive << runnableCompleted_event;
		dont_initialize();
		SC_THREAD(periodicRunnablesReleaser_thread);
		SC_METHOD(nonPeriodicIndependentRunnablesReleaser_method);
		sensitive << iteration_event;

		// Start mode switching thread if required
		if (simulationEnd != 0) {
			SC_THREAD(modeSwitcher_thread);
		}
	}

	// Create the tiles making the platform
	for (int x = 0; x < CommonParameter::dim_x; x++) {
		for (int y = 0; y < CommonParameter::dim_y; y++) {

			// Create tile
			string label = "Tile[" + int_to_str(x) + "][" + int_to_str(y) + "]";
			tile[x][y] = new Tile(label.data(), x, y);
			tile[x][y]->proc->initialize(x, y);
			tile[x][y]->router->initialize(x, y);
			tile[x][y]->initialize();
			tile[x][y]->clk(clk);
			tile[x][y]->reset(reset);

			// Bind all interconnect signals
			for (int i = 0; i < ROUTER_PORTS - 1; i++) {
				switch (i) {
				case (WEST):
					tile[x][y]->valid_in[i](W_valid_in[x][y]);
					tile[x][y]->flit_in[i](W_flit_in[x][y]);
					for (int k = 0; k < MAX_N_VCS; k++) {
						tile[x][y]->in_vc_buffer_rd[i][k](
								W_in_vc_buffer_rd[x][y][k]);
					}

					tile[x][y]->valid_out[i](W_valid_out[x][y]);
					tile[x][y]->flit_out[i](W_flit_out[x][y]);
					for (int k = 0; k < MAX_N_VCS; k++) {
						tile[x][y]->out_vc_buffer_rd[i][k](
								W_out_vc_buffer_rd[x][y][k]);
					}
					break;
				case (EAST):
					tile[x][y]->valid_in[i](W_valid_out[x + 1][y]);
					tile[x][y]->flit_in[i](W_flit_out[x + 1][y]);
					for (int k = 0; k < MAX_N_VCS; k++) {
						tile[x][y]->in_vc_buffer_rd[i][k](
								W_out_vc_buffer_rd[x + 1][y][k]);
					}

					tile[x][y]->valid_out[i](W_valid_in[x + 1][y]);
					tile[x][y]->flit_out[i](W_flit_in[x + 1][y]);
					for (int k = 0; k < MAX_N_VCS; k++) {
						tile[x][y]->out_vc_buffer_rd[i][k](
								W_in_vc_buffer_rd[x + 1][y][k]);
					}
					break;
				case (NORTH):
					tile[x][y]->valid_in[i](N_valid_in[x][y]);
					tile[x][y]->flit_in[i](N_flit_in[x][y]);
					for (int k = 0; k < MAX_N_VCS; k++) {
						tile[x][y]->in_vc_buffer_rd[i][k](
								N_in_vc_buffer_rd[x][y][k]);
					}

					tile[x][y]->valid_out[i](N_valid_out[x][y]);
					tile[x][y]->flit_out[i](N_flit_out[x][y]);
					for (int k = 0; k < MAX_N_VCS; k++) {
						tile[x][y]->out_vc_buffer_rd[i][k](
								N_out_vc_buffer_rd[x][y][k]);
					}
					break;
				case (SOUTH):
					tile[x][y]->valid_in[i](N_valid_out[x][y + 1]);
					tile[x][y]->flit_in[i](N_flit_out[x][y + 1]);
					for (int k = 0; k < MAX_N_VCS; k++) {
						tile[x][y]->in_vc_buffer_rd[i][k](
								N_out_vc_buffer_rd[x][y + 1][k]);
					}

					tile[x][y]->valid_out[i](N_valid_in[x][y + 1]);
					tile[x][y]->flit_out[i](N_flit_in[x][y + 1]);
					for (int k = 0; k < MAX_N_VCS; k++) {
						tile[x][y]->out_vc_buffer_rd[i][k](
								N_in_vc_buffer_rd[x][y + 1][k]);
					}
					break;
				default:
					;
				}
			}

			// Setup communication between platform and cores
			DcProc* proc = static_cast<DcProc*>(tile[x][y]->proc);
			proc->newRunnable_event = &newRunnable_event.at(x).at(y);
			proc->sched = sched;
			proc->runnableCompleted_event = &runnableCompleted_event;
			proc->nocTrafficCsvFile = &nocTrafficCsvFile;
			proc->runnablesCsvFile = &runnablesCsvFile;
			proc->instsCsvFile = &instsCsvFile;
			proc->priorities = application->GetAllPriorities(taskGraph);
		}
	} // End platform creation
}

/**
 * SystemC method used to get completed runnables from PEs and to enable
 * runnables dependent on the completed ones.
 */
void DcPlatform::dependentRunnablesReleaser_method() {

	// Copy all completed runnables from PEs into dcSystem and erase them in PE_XY
	vector<dcRunnableInstance *> completedRunInstances;
	for (int x = 0; x < CommonParameter::dim_x; x++) {
		for (int y = 0; y < CommonParameter::dim_y; y++) {
			DcProc* proc = static_cast<DcProc*>(tile[x][y]->proc);
			for (std::vector<dcRunnableInstance *>::const_iterator i =
					proc->completedRunnableInstances.begin();
					i != proc->completedRunnableInstances.end(); ++i) {
				completedRunInstances.push_back(*i);
				nbRunnablesCompleted++;
			}
		}
	}
	cout << nbRunnablesCompleted << " completed and "
				<< nbRunnablesMapped << " mapped at time " << sc_time_stamp() << endl;

	// When we don't handle periodics stop the simulation when all
	// the runnables (periodic and non periodic) have been executed once
	// or start a new iteration if needed
	if (simulationEnd == 0
			&& (DCParameters::dontHandlePeriodic || hyperPeriod == 0)) {
		if (nbRunnablesCompleted / runnables.size()
				>= DCParameters::iterations) {
			stopSimu_event.notify();
			return;
		}
		if ((nbRunnablesCompleted % runnables.size()) == 0) {
			iteration_event.notify(0, SC_NS);
		}
	}

	// Loop over all the completed runnables
	for (vector<dcRunnableInstance *>::size_type i = 0;
			i < completedRunInstances.size(); ++i) {
		// Loop over all runnable that depend on the completed one
		dcRunnableInstance *runInst = completedRunInstances.at(i);
		dcRunnableCall* runCall = runInst->getRunCall();
		vector<dcRunnableCall*> enabledToExecute = runCall->GetListOfEnables();
		for (std::vector<dcRunnableCall*>::iterator it =
				enabledToExecute.begin(); it != enabledToExecute.end(); ++it) {
			// If all the previous runnables of the current dependent are completed, release it (the dependent one)
			if ((*it)->GetEnabledBy() == (*it)->GetListOfEnablers().size()) {
				dcRunnableInstance *runInst = new dcRunnableInstance(*it);
				releaseRunnable(runInst);
			}
			// Else only indicate that one dependency has been satisfied
			else {
				(*it)->SetEnabledBy((*it)->GetEnabledBy() + 1);
			}
		}
	}

	// Delete all completed runnable instances
	for (int x = 0; x < CommonParameter::dim_x; x++) {
		for (int y = 0; y < CommonParameter::dim_y; y++) {
			DcProc* proc = static_cast<DcProc*>(tile[x][y]->proc);
			for (std::vector<dcRunnableInstance *>::const_iterator i =
					proc->completedRunnableInstances.begin();
					i != proc->completedRunnableInstances.end(); ++i) {
				delete (*i);
			}
			proc->completedRunnableInstances.clear();
		}
	}
}

/**
 * SystemC method called once only during initialization.
 */
void DcPlatform::labelsMapper_method() {
	for (vector<dcLabel*>::size_type i = 0; i < labels.size(); i++) {
		dcMappingHeuristicI::dcMappingLocation loc = mappingHeuristic->mapLabel(
				labels.at(i)->GetID(), sc_time_stamp().value() * 1E-3,
				labels.at(i)->GetName());
		// All the PEs have a local copy of the labels mapping table
		for (int row(0); row < CommonParameter::dim_x; ++row) {
			for (int col(0); col < CommonParameter::dim_y; ++col) {
				DCParameters::labelsMap[labels.at(i)->GetID()] = loc;
			}
		}
	}
}

/**
 * This SC_METHOD is called each time a mode switch occur
 * using dynamic sensitivity
 */
void DcPlatform::modeSwitcher_thread() {
	vector<mode_t>::size_type modIdx = 0;
	while (true) {
		unsigned long int nowInNano = sc_time_stamp().value() * 1E-3;
		if (modes.at(modIdx).name.compare("end")) {
			mappingHeuristic->switchMode(nowInNano, modes.at(modIdx).file,
					modes.at(modIdx).name);
			labelsMapper_method();
			updateInstructionsExecTimeBounds(modes.at(modIdx).file);
		}
		assert(
				modes.at(modIdx).time == nowInNano
						&& "Invalid time in modeSwitcher_thread");
		if (modIdx + 1 < modes.size()) {
			unsigned long int waitTime = modes.at(modIdx + 1).time - nowInNano;
			modIdx++;
			wait(waitTime, SC_NS);
		} else {
			stopSimu_event.notify();
			break;
		}
	}
}

/**
 * SC_METHOD that release all independent runnables.
 * This method is called first at time zero and then on each
 * iteration_event notification.
 */
void DcPlatform::nonPeriodicIndependentRunnablesReleaser_method() {
	vector<dcRunnableCall *> runs;
	if (DCParameters::dontHandlePeriodic) {
		runs = application->GetIndependentRunnables(taskGraph);
	} else {
		runs = application->GetIndependentNonPeriodicRunnables(taskGraph);
	}
	if (DCParameters::dontHandlePeriodic && runs.size() == 0) {
		cerr
				<< "app doesn't contain any independent runnable to start, please check it !"
				<< endl;
		exit(-1);
	}
	for (std::vector<dcRunnableCall *>::iterator it = runs.begin();
			it != runs.end(); ++it) {
		dcRunnableInstance * runInst = new dcRunnableInstance(*it);
		releaseRunnable(runInst);
	}
}

/**
 * SystemC thread used to release periodic runnables.
 * This thread is triggered at initialization time and then dynamically
 * computes its next occurrence.
 */
void DcPlatform::periodicRunnablesReleaser_thread() {

	// Only used in periodic handling case
	if (DCParameters::dontHandlePeriodic) {
		return;
	}

	vector<int>::size_type nbPeriodicRunnables =
			periodicAndSporadicRunnables.size();
	vector<unsigned long int> remainingTimeBeforeRelease(nbPeriodicRunnables);
	for (vector<int>::size_type i = 0; i < nbPeriodicRunnables; ++i) {
		dcRunnableCall *runnable = periodicAndSporadicRunnables.at(i);
		remainingTimeBeforeRelease[i] = runnable->GetOffsetInNano();
	}

	while (true) {

		// Stop the simu if hyper period reached and we are not using mode switch
		if (simulationEnd == 0 && hyperPeriod > 0
				&& (sc_time_stamp().value() / 1E3) >= hyperPeriod) {
			stopSimu_event.notify();
			return;
		}

		unsigned long int waitTime = ULONG_MAX;
		for (vector<int>::size_type i = 0; i < nbPeriodicRunnables; ++i) {
			if (remainingTimeBeforeRelease[i] == 0) {
				dcRunnableCall *runnable = periodicAndSporadicRunnables.at(i);
				dcRunnableInstance * runInst = new dcRunnableInstance(runnable);
				releaseRunnable(runInst);
				remainingTimeBeforeRelease[i] = runnable->GetPeriodInNano();
			}
			if (remainingTimeBeforeRelease[i] < waitTime) {
				waitTime = remainingTimeBeforeRelease[i];
			}
		}
		for (vector<int>::size_type i = 0; i < nbPeriodicRunnables; ++i) {
			remainingTimeBeforeRelease[i] = remainingTimeBeforeRelease[i]
					- waitTime;
		}
		wait(waitTime, SC_NS);
	}
}

void DcPlatform::releaseRunnable(dcRunnableInstance *runnable) {
	releasedRunnables.push_back(runnable);
	runnableReleased_event.notify();
}

void DcPlatform::runnablesDispatcher_thread() {

	int appIterations = 0;
	int remainingNbRunnablesToMap = runnables.size();

	// Map runnables until application is "finished"
	while (true) {

		// Wait until a runnable can be executed
		if (releasedRunnables.empty()) {
			wait();
			continue;
		}

		// Ask the mapping heuristic where to map the current ready runnable
		dcRunnableCall * runCall = releasedRunnables.front()->getRunCall();
		dcMappingHeuristicI::dcMappingLocation pe =
				mappingHeuristic->mapRunnable(sc_time_stamp().value(),
						runCall->GetRunClassId(), runCall->GetRunClassName(),
						runCall->GetTask()->GetName(),
						runCall->GetTask()->GetID(), runCall->GetIdInTask(),
						releasedRunnables.front()->GetPeriodId());
		int x = pe.first;
		int y = pe.second;
		DcProc* proc = static_cast<DcProc*>(tile[x][y]->proc);

		// If the PE has completed receiving the previous runnable,
		// map the new one on it
		// ONE clock cycle
		if (!proc->newRunnableSignal) {
			nbRunnablesMapped++;
			releasedRunnables.front()->SetMappingTime(sc_time_stamp().value());
//			cerr << "mapping " << releasedRunnables.front()->GetName()
//					<< " on PE" << x << y << " at time "
//					<< sc_time_stamp().value() * 1E-3 << endl;
			proc->newRunnable = releasedRunnables.front();
			proc->newRunnableSignal = true;
			newRunnable_event[x][y].notify();
			releasedRunnables.erase(releasedRunnables.begin());
			remainingNbRunnablesToMap--;
			if (remainingNbRunnablesToMap == 0) {
				appIterations++;
				remainingNbRunnablesToMap = runnables.size();
			}
			wait(1, SC_NS);
		}

		// Else move the current runnable to the end of the independent queue
		// ONE clock cycle
		else {
			releasedRunnables.push_back(releasedRunnables.front());
			releasedRunnables.erase(releasedRunnables.begin());
			wait(1, SC_NS);
		}
	}
}

void DcPlatform::stopSimu_thread() {

	// In the case of periodic runnables
	// handling,  wait for all mapped ones to be
	// completed
	if (!DCParameters::dontHandlePeriodic) {
		while (nbRunnablesCompleted < nbRunnablesMapped) {
			wait(runnableCompleted_event);
		}
	}

	// Get end time
	clock_t end = std::clock();
	unsigned long int endTimeInNano = sc_time_stamp().value() / 1E3;

	// Print some results on stdout
	cout << " ##Simulation Results##" << endl << endl;
	cout << "    Number of runnable instances executed         : "
			<< nbRunnablesCompleted << endl;
	if (DCParameters::dontHandlePeriodic) {
		cout << "    Completed application iterations              : "
				<< (nbRunnablesCompleted / runnables.size()) << endl;
	}
	cout << "    Execution time of the application             : "
			<< endTimeInNano << " ns" << endl;
	int nbDeadlineMisses = 0;
	for (int x(0); x < CommonParameter::dim_x; ++x) {
		for (int y(0); y < CommonParameter::dim_y; ++y) {
			DcProc* proc = static_cast<DcProc*>(tile[x][y]->proc);
			nbDeadlineMisses = nbDeadlineMisses + proc->deadlinesMissed;
		}
	}
	cout << "    Number of runnables which missed deadlines    : "
			<< nbDeadlineMisses << endl;
	int pkts = 0;
	for (int x(0); x < CommonParameter::dim_x; ++x) {
		for (int y(0); y < CommonParameter::dim_y; ++y) {
			DcProc* proc = static_cast<DcProc*>(tile[x][y]->proc);
			pkts = pkts + proc->injected_packet_count;
		}
	}
	cout << "    Number of packets exchanged through NoC       : " << pkts
			<< endl;
	cout << "    Simulation time                               : "
			<< (double) (end - start) / CLOCKS_PER_SEC << " seconds" << endl;
	cout << "    Avg number of packets exchanged per NoC link  : 1.0" << endl;
	cout << "    RSD number of packets exchanged per NoC link  : 1.0" << " %"
			<< endl << endl;
	sc_stop();
}

void DcPlatform::updateInstructionsExecTimeBounds(string newAppFile) {

	// Parse new application
	dcAmaltheaParser amaltheaParser;
	AmApplication* amApplication = new AmApplication();
	amaltheaParser.ParseAmaltheaFile(newAppFile, amApplication);
	dcApplication *app = new dcApplication();
	dcTaskGraph * taskGraph = app->createGraph("dcTaskGraph");
	app->CreateGraphEntities(taskGraph, amApplication, DCParameters::seqDep);

	// Compare and update instructions
	vector<dcInstruction *> instructions = application->GetAllInstructions(
			taskGraph);
	vector<dcInstruction *> newInstructions = app->GetAllInstructions(
			taskGraph);
	for (vector<dcInstruction *>::size_type i = 0; i < instructions.size();
			i++) {
		dcInstruction *newInst = newInstructions.at(i);
		dcInstruction *oldInst = instructions.at(i);
		if (newInst->GetName() != oldInst->GetName()) {
			cerr << "error while updating instructions execution time bounds"
					<< endl;
			exit(-1);
		}
		string instrName = newInst->GetName();
		if (instrName == "sw:InstructionsDeviation") {
			dcExecutionCyclesDeviationInstruction *newInstDev =
					static_cast<dcExecutionCyclesDeviationInstruction*>(newInst);
			dcExecutionCyclesDeviationInstruction *oldInstDev =
					static_cast<dcExecutionCyclesDeviationInstruction*>(oldInst);
			oldInstDev->SetLowerBound(newInstDev->GetLowerBound());
			oldInstDev->SetUpperBound(newInstDev->GetUpperBound());
			oldInstDev->SetKappa(newInstDev->GetKappa());
			oldInstDev->SetLambda(newInstDev->GetLambda());
			oldInstDev->SetMean(newInstDev->GetMean());
			oldInstDev->SetRemainPromille(newInstDev->GetRemainPromille());
			oldInstDev->SetSD(newInstDev->GetSD());
		}
		if (instrName == "sw:InstructionsConstant") {
			dcExecutionCyclesConstantInstruction *newInstCst =
					static_cast<dcExecutionCyclesConstantInstruction*>(newInst);
			dcExecutionCyclesConstantInstruction *oldInstCst =
					static_cast<dcExecutionCyclesConstantInstruction*>(oldInst);
			oldInstCst->SetValue(newInstCst->GetValue());
		}
	}

	// Delete objects
	delete taskGraph;
	delete app;
	delete amApplication;
}
