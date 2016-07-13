#include <map>
#include <random>
#include "dc_proc.h"
#include "dc_parameters.h"
#include "commons/parser/dcExecutionCyclesConstantInstruction.h"
#include "commons/parser/dcExecutionCyclesDeviationInstruction.h"
#include "../proc_functions.h"

// Number of bytes in a packet (considering a given flit size in bits,
// this also defines the number of flits)
#define PACKET_SIZE_IN_BYTES 32

// Flit size in bits
#define FLIT_SIZE_IN_BITS 32

#define NB_FLITS_PER_PACKET ((PACKET_SIZE_IN_BYTES * 8) / FLIT_SIZE_IN_BITS)

int DcProc::nbRunBlocked = 0;
int DcProc::packetId = 0;
int DcProc::writeRequestId = 0;
int DcProc::readRequestId = 0;

/**
 * Add the given runnable execution element to th ready list.
 * This function preserve ready runnables list order according
 * to the priority of the runnable execution element.
 */
void DcProc::addReadyRunnable(runnableExecElement runnableExecElem) {
	readyRunnables.insert(readyRunnables.begin(), runnableExecElem);
	std::sort(readyRunnables.begin(), readyRunnables.end(),
			[](const runnableExecElement &left, const runnableExecElement &right)
			{
				return left.priority < right.priority;
			});
}

/**
 * Add the given run blocked object to the list of blocked runnable.
 * Check if the runnable instance is not already blocked.
 */
void DcProc::addBlockedRunnable(runnableBlockedOnRemoteRead blocked) {
	vector<runnableBlockedOnRemoteRead>::iterator already = std::find_if(
			runnablesBlockedOnRemoteRead.begin(),
			runnablesBlockedOnRemoteRead.end(),
			[&blocked](runnableBlockedOnRemoteRead& o)
			{
				return o.execElem.runInstance == blocked.execElem.runInstance;
			});
	if (already != runnablesBlockedOnRemoteRead.end()) {
		cerr << "ERORRRR " << blocked.execElem.runInstance->GetUniqueID()
				<< " already blocked on PE" << local_x << local_y << endl;
		exit(-1);
	}
	runnablesBlockedOnRemoteRead.push_back(blocked);
}

ProcEvaluationFactors *DcProc::evaluation() {
	return NULL;
}

/**
 * Execute the given constant number of instructions by
 * waiting. Also log information for energy model.
 */
void DcProc::executeInstructionsConstant(dcInstruction *inst,
		dcRunnableInstance *run, int instructionId) {
	dcExecutionCyclesConstantInstruction* einst =
			static_cast<dcExecutionCyclesConstantInstruction*>(inst);
	double waitTimeInNano = 1E9 * einst->GetValue()
			* type->getNbCyclesPerInstructions() / type->getFrequencyInHz();
	*instsCsvFile << run->getRunCall()->GetRunClassId() << ",";
	*instsCsvFile << instructionId << ",";
	*instsCsvFile << einst->GetValue() << endl;
	wait(waitTimeInNano, SC_NS);
}

/**
 * Execute the given deviation number of instructions by
 * waiting. Also log information for energy model.
 */
void DcProc::executeInstructionsDeviation(dcInstruction *inst,
		dcRunnableInstance *run, int instructionId) {
	dcExecutionCyclesDeviationInstruction* einst =
			static_cast<dcExecutionCyclesDeviationInstruction*>(inst);
	double UpperBound = 0.0;
	double LowerBound = 0.0;
	if (einst->GetUpperBoundValid()) {
		UpperBound = einst->GetUpperBound();
	}
	if (einst->GetLowerBoundValid()) {
		LowerBound = einst->GetLowerBound();
	}
	std::uniform_int_distribution<> distr(LowerBound, UpperBound);
	int compute_duration = distr(gen);
	double waitTimeInNano = 1E9 * compute_duration
			* type->getNbCyclesPerInstructions() / type->getFrequencyInHz();
	*instsCsvFile << run->getRunCall()->GetRunClassId() << ",";
	*instsCsvFile << instructionId << ",";
	*instsCsvFile << compute_duration << endl;
	wait(waitTimeInNano, SC_NS);
}

void DcProc::executeRemoteLabelRead(dcRemoteAccessInstruction *rinst,
		dcRunnableInstance *run, int x, int y, int instructionId,
		int priority) {

	// Create the packet representing read request
	unsigned long int nowInNano = sc_time_stamp().value() * 1E-3;
	Flit *head = create_head_flit_fixed_dest(local_x, local_y, x, y, nowInNano);
	int nbPacketsToBeReceived = (int) (ceil(
			double(rinst->GetLabel()->GetSize())
					/ double(8 * PACKET_SIZE_IN_BYTES)));
	head->readRequestId = readRequestId++;
	head->readRequestSize = nbPacketsToBeReceived;
	head->readResponse = false;
	head->packetId = packetId++;
	head->priority = priority;
	Packet packet(head, NB_FLITS_PER_PACKET);
	// push all its flits to the source_queue
	for (int i = 0; i < NB_FLITS_PER_PACKET; i++) {
		source_queue.push(*(packet.flit[i])); // push value of flit to queue, not pointer
	}

	// Put the runnable in the blocked list
	// and remove it from the ready list
	runnableBlockedOnRemoteRead blockedOnRemoteRead;
	runnableExecElement execElem;
	execElem.instructionIdx = instructionId + 1;
	execElem.runInstance = run;
	execElem.priority = priority;
	blockedOnRemoteRead.nbPktReceived = 0;
	blockedOnRemoteRead.nbPktToBeReceived = nbPacketsToBeReceived;
	blockedOnRemoteRead.readRequestId = head->readRequestId;
	blockedOnRemoteRead.execElem = execElem;
	blockedOnRemoteRead.xReplier = x;
	blockedOnRemoteRead.yReplier = y;
	addBlockedRunnable(blockedOnRemoteRead);
	removeReadyRunnable(run);

	nbRunBlocked++;
#ifdef DEBUG
	cerr << "time " << sc_time_stamp() << ": " << run->GetUniqueID()
	<< " blocked: blocked =  " << nbRunBlocked << " on PE" << local_x
	<< local_y << ": first blocked is "
	<< runnablesBlockedOnRemoteRead.front().execElem.runInstance->GetUniqueID()
	<< endl;
#endif

	// Sending a packet has a cost of 32 cycles for now:
	// 1 clock cycle for each flit of the packet
	wait(PACKET_SIZE_IN_BYTES, SC_NS);
}

void DcProc::executeRemoteLabelWrite(dcRemoteAccessInstruction *rinst,
		dcRunnableInstance *run, int x, int y) {

	// We consider the size of labels in Amalthea is in bits.
	int number_of_Packets = (int) (ceil(
			double(rinst->GetLabel()->GetSize())
					/ double(8 * PACKET_SIZE_IN_BYTES)));
	unsigned long int nowInNano = sc_time_stamp().value() * 1E-3;

	// Send each packet
	for (int pkts = 0; pkts < number_of_Packets; pkts++) {
		Flit *head = create_head_flit_fixed_dest(local_x, local_y, x, y,
				nowInNano);
		head->packetId = packetId++;
		head->readRequestId = -1;
		head->priority = run->getRunCall()->GetPriority();
		Packet packet(head, NB_FLITS_PER_PACKET);
		// push all its flits to the source_queue
		for (int i = 0; i < NB_FLITS_PER_PACKET; i++) {
			source_queue.push(*(packet.flit[i])); // push value of flit to queue, not pointer
		}
		wait(PACKET_SIZE_IN_BYTES, SC_NS);
	}
}

/*
 * Method used to receive flit. This method is sensitive to the clock.
 */
void DcProc::flitIn_method() {

	// If a new flit is there
	if (valid_in) {

		// Get flit and check vc
		Flit f = flit_in.read();
		int vc_id = f.vc_id;
		if (vc_id == -1) {
			cerr << "VC ID == -1" << endl;
			exit(-1);
		}

		// If flit is head, save it to build packet back
		if (f.head) {
			lastHead[vc_id] = f;
		}

		// Indicate to local router the flit that we read
		for (int vi = 0; vi < RouterParameter::n_VCs; vi++) {
			if (vi == vc_id) {
				in_vc_buffer_rd[vi].write(true);
			} else {
				in_vc_buffer_rd[vi].write(false);
			}
		}

		// If a packet has been completely received, handle it
		if (f.tail) {
			Packet p(&(lastHead[vc_id]), NB_FLITS_PER_PACKET);
			receivePkt(p);
		}
	}

	// Else
	else {
		// Indicate to local router that we didn't read any virtual channel
		for (int vi = 0; vi < RouterParameter::n_VCs; vi++) {
			in_vc_buffer_rd[vi].write(false);
		}
	}
}

/*
 * Method used to send flit. This method is sensitive to the clock.
 */
void DcProc::flitOut_method() {

	// Send a flit if something to send AND room in VC channel
	int sentVc = -1;
	if (source_queue.size() > 0) {

		Flit f = source_queue.front();

		// Virtual channel selection based on priority:
		unsigned int normalizedPrio;
		for (normalizedPrio = 1; normalizedPrio <= priorities.size();
				normalizedPrio++) {
			if (f.priority == priorities[normalizedPrio]) {
				break;
			}
		}
		if (normalizedPrio > priorities.size()) {
			cerr << "priority not found in list of priorities" << endl;
			exit(-1);
		}
		double ratio = ((double) priorities.size()) / RouterParameter::n_VCs;
		if (ratio <= 1) {
			f.vc_id = normalizedPrio;
		} else {
			f.vc_id = (int) (ceil(normalizedPrio / ratio)) - 1;
		}
		if (f.vc_id >= RouterParameter::n_VCs || f.vc_id < 0) {
			cerr << "error in computing VC: prio = " << f.priority << " among "
					<< priorities.size() << " priorities, normalized prio = "
					<< normalizedPrio << ", ratio = " << ratio << ", n_VCs = "
					<< RouterParameter::n_VCs << ", vc = " << f.vc_id << endl;
			exit(-1);
		}

		// Send the packet if local router not full
		if (out_vc_remain[f.vc_id] >= 1) {
			if (f.head) {
				injected_packet_count++;
			}
			count_minus[f.vc_id].write(true);
			valid_out.write(true);
			flit_out.write(f);
			source_queue.pop();
			sentVc = f.vc_id;
		}
	}

	// Send null flit if no flit sent
	if (sentVc == -1) {
		valid_out.write(false);
		flit_out.write(Flit());
	}

	// Update count minus
	for (int vc = 0; vc < RouterParameter::n_VCs; vc++) {
		if (vc != sentVc) {
			count_minus[vc].write(false);
		}
	}
}

void DcProc::initialize(int x, int y) {
	local_x = x;
	local_y = y;
}

void DcProc::receivePkt(const Packet & p) {

	// Assert the packet is delivered to correct PE
	Flit *head = p.flit[0];
	if (head->dst_x != local_x || head->dst_y != local_y) {
		cerr << "PE" << local_x << local_y << " received packet from PE"
				<< head->src_x << head->src_y << " targeted to PE"
				<< head->dst_x << head->dst_y << ": " << endl << "  " << *head
				<< endl;
		exit(-1);
	}

	// Save packet statistic
	*nocTrafficCsvFile << head->packetId << ",";
	*nocTrafficCsvFile << head->readRequestId << ",";
	*nocTrafficCsvFile << head->priority << ",";
	*nocTrafficCsvFile << "(" << head->src_x << " " << head->src_y << "),";
	*nocTrafficCsvFile << "(" << head->dst_x << " " << head->dst_y << "),";
	*nocTrafficCsvFile << fixed << head->injected_time << ",";
	unsigned long int t = (sc_time_stamp().value() / 1000);
	*nocTrafficCsvFile << fixed << t << ",";
	*nocTrafficCsvFile << fixed << (t - head->injected_time) << ",";
	*nocTrafficCsvFile << "NA";
	*nocTrafficCsvFile << endl;

	// We receive a read request from a remote PE
	// we must send back the response
	if (head->readRequestId != -1 && !head->readResponse) {
		unsigned long int nowInNano = sc_time_stamp().value() * 1E-3;
		for (int pkts = 0; pkts < head->readRequestSize; pkts++) {
			Flit *h = create_head_flit_fixed_dest(local_x, local_y, head->src_x,
					head->src_y, nowInNano);
			h->packetId = packetId++;
			h->readRequestId = head->readRequestId;
			h->readRequestSize = head->readRequestSize;
			h->readResponse = true;
			h->priority = head->priority;
			Packet packet(h, NB_FLITS_PER_PACKET);
			// push all its flits to the source_queue
			for (int i = 0; i < NB_FLITS_PER_PACKET; i++) {
				source_queue.push(*(packet.flit[i]));
			}
		}
	}

	// We receive a response from a previous remote read originated from this PE
	// We must move the runnable concerned by this read from the "blocked on remote read"
	// runnables list to the "ready" runnables one.
	else if (head->readRequestId != -1 && head->readResponse) {

		vector<runnableBlockedOnRemoteRead>::iterator it;
		it =
				std::find_if(runnablesBlockedOnRemoteRead.begin(),
						runnablesBlockedOnRemoteRead.end(),
						[&head](const runnableBlockedOnRemoteRead& elem)
						{
							return ((elem.xReplier == head->src_x && elem.yReplier == head->src_y) &&
									(elem.readRequestId == head->readRequestId));
						});

		// If this PE doesn't have a runnable blocked on the remote read
		if (it == runnablesBlockedOnRemoteRead.end()) {
			cerr << "PE" << local_x << local_y
					<< " received a read response from PE" << head->src_x
					<< head->src_y
					<< " for a packet that he didn't requested which ID is "
					<< head->readRequestId << endl;
			exit(-1);
		}

		// We receive a new packet for the remote read
		// If all the packets have been received, then we can
		// unblock the runnable.
		(it->nbPktReceived)++;
		if (it->nbPktReceived == it->nbPktToBeReceived) {
			addReadyRunnable(it->execElem);
			//if (it->execElem.runInstance->getRunCall()->GetRunClassName()
			//		== "runnable00264") {
#ifdef DEBUG
			cerr << "time " << sc_time_stamp() << ": "
			<< it->execElem.runInstance->GetUniqueID()
			<< " unblocked: blocked =  " << nbRunBlocked << endl;
#endif
			//}
			runnablesBlockedOnRemoteRead.erase(it);
			newRunnable_event->notify();
			nbRunBlocked--;
		}
	}

	else if (head->readRequestId == -1) {
		// nothing to do with write requests for now
	}

	else {
		cout << "ERROR in received packet " << endl;
		exit(-1);
	}
}

void DcProc::removeReadyRunnable(dcRunnableInstance *runnable) {
	vector<runnableExecElement>::iterator toRemove = std::find_if(
			readyRunnables.begin(), readyRunnables.end(),
			[&runnable](runnableExecElement& elem)
			{
				return elem.runInstance == runnable;
			});
	if (toRemove == readyRunnables.end()) {
		cerr << "ERORRRR " << runnable->getRunCall()->GetRunClassName()
				<< " not found" << endl;
		exit(-1);
	}
	readyRunnables.erase(toRemove);
}

/**
 * SystemC thread that execute an instruction of the current
 * runnable and then check for preemption.
 */
void DcProc::runnableExecuter_thread() {

	while (true) {

		// Wait for runnables from dcSystem
		if (readyRunnables.empty() && !newRunnableSignal) {
			wait(*newRunnable_event);
		}

		// If a new runnable has been sent during the execution of the last instruction
		// Put it in the ready list
		// ONE clock cycle
		if (newRunnableSignal) {
			int prio;
			if (sched == PRIO) {
				prio = newRunnable->getRunCall()->GetPriority();
			} else if (sched == FCFS) {
				prio = newRunnable->GetMappingTime();
			}
			runnableExecElement execElement;
			execElement.instructionIdx = 0;
			execElement.runInstance = newRunnable;
			execElement.priority = prio;
			newRunnable->SetCoreReceiveTime(sc_time_stamp().value());
			addReadyRunnable(execElement);
			newRunnableSignal = false;
			wait(1, SC_NS);
		}

		// Choose the runnable with the highest priority among ready ones
		// Operation in ZERO clock cycle
		runnableExecElement execElem = readyRunnables.front();
		dcRunnableInstance *currentRunnable = execElem.runInstance;
		unsigned int instructionId = execElem.instructionIdx;
		std::vector<DCApplication::dcInstruction*> allInstructions =
				currentRunnable->getRunCall()->GetAllInstructions();
		unsigned int nbInstructions = allInstructions.size();
		bool blockedOnRemoteRead = false;

		// Move to next instruction for next time this runnable will be executed
		readyRunnables.front().instructionIdx++;

		// If the next instruction to execute exists.
		// This checks is required because when we block a runnable
		// on a remote read and its the last instruction, when we unblock it
		// we are above the last instruction
		if (instructionId < nbInstructions) {

			dcInstruction* inst = allInstructions.at(instructionId);
			string instrName = inst->GetName();
			unsigned long int nowInPico = sc_time_stamp().value();
			if (instructionId == 0) {
				currentRunnable->SetStartTime(nowInPico);
			}

			// Label accesses
			if (instrName == "sw:LabelAccess") {

				// Search where is the label
				dcRemoteAccessInstruction *rinst =
						static_cast<dcRemoteAccessInstruction*>(inst);
				string labelId = rinst->GetLabel()->GetID();
				map<string,
						dreamcloud::platform_sclib::dcMappingHeuristicI::dcMappingLocation>::const_iterator labelToLoc =
						DCParameters::labelsMap.find(labelId);
				if (labelToLoc != DCParameters::labelsMap.end()) {
					int destX = labelToLoc->second.first;
					int destY = labelToLoc->second.second;

					// Local read and write accesses
					// ONE clock cycle per byte
					if ((local_x == destX) && (local_y == destY)) {
						int localAccessSize = (int) (ceil(
								double(rinst->GetLabel()->GetSize())
										/ double(8)));
						wait(localAccessSize, SC_NS);
					}

					// Remote write: sent packets according to the size of the label
					// nbPackets * PACKET_SIZE clock cycles
					else if (rinst->GetWrite()) {
						executeRemoteLabelWrite(rinst, currentRunnable, destX,
								destY);
					}

					// Remote read: send one packet including information allowing
					// receiver to send response back
					// PACKET_SIZE clock cycles
					else if (!rinst->GetWrite()) {
						executeRemoteLabelRead(rinst, currentRunnable, destX,
								destY, instructionId, execElem.priority);
						blockedOnRemoteRead = true;
					}
				} // End label access
				else {
					cerr << "Error label not found" << endl;
					exit(-1);
				}
			}

			// Instruction constant
			else if (instrName == "sw:InstructionsConstant") {
				executeInstructionsConstant(inst, currentRunnable,
						instructionId);
			} else if (instrName == "sw:InstructionsDeviation") {
				executeInstructionsDeviation(inst, currentRunnable,
						instructionId);
			}
		} // End if instructionID < nbInstructions

		// Check if runnable is completed
		if (!blockedOnRemoteRead
				&& (instructionId >= nbInstructions - 1 || nbInstructions == 0)) {

			currentRunnable->SetCompletionTime(sc_time_stamp().value());
			unsigned long int runnableExecutionTime =
					currentRunnable->GetCompletionTime()
							- currentRunnable->GetCoreReceiveTime();
			// Check deadline miss
			if (runnableExecutionTime * 1E-3
					> currentRunnable->getRunCall()->GetDeadlineValueInNano()) {
				deadlinesMissed++;
			}

			// Add the completed runnable to completed queue and remove it from ready one
			completedRunnableInstances.push_back(currentRunnable);
			(*runnableCompleted_event).notify();
			removeReadyRunnable(currentRunnable);

			// Save information in output file
			*runnablesCsvFile << "(" << local_x << " " << local_y << "),";
			*runnablesCsvFile
					<< currentRunnable->getRunCall()->GetRunClassName() << ",";
			*runnablesCsvFile << currentRunnable->getRunCall()->GetRunClassId()
					<< ",";
			*runnablesCsvFile << currentRunnable->getRunCall()->GetPriority()
					<< ",";
			unsigned int long l = currentRunnable->GetMappingTime() / 1000;
			*runnablesCsvFile << fixed << l << ",";
			l = currentRunnable->GetStartTime() / 1000;
			*runnablesCsvFile << fixed << l << ",";
			l = currentRunnable->GetCompletionTime() / 1000;
			*runnablesCsvFile << fixed << l << ",";
			l = currentRunnable->GetCompletionTime() / 1000
					- currentRunnable->GetMappingTime() / 1000;
			*runnablesCsvFile << fixed << l << ",";
			*runnablesCsvFile << fixed
					<< currentRunnable->getRunCall()->GetDeadlineValueInNano()
					<< ",";
			*runnablesCsvFile
					<< currentRunnable->getRunCall()->GetDeadlineValueInNano()
							- l << endl;
		}
	}
}

/*
 * update out_vc_remain of all VCs
 */
void DcProc::out_vc_remain_method() {
	for (int vo = 0; vo < RouterParameter::n_VCs; vo++) {
		int tmp = out_vc_remain_reg[vo];
		if (count_minus[vo].read())
			tmp -= 1;
		if (count_plus[vo].read())
			tmp += 1;
		if (tmp < 0) {
			cerr << "Error out_vc_remain[" << vo << "] is negative" << endl;
			exit(-1);
		}
		out_vc_remain[vo] = tmp;
	}
}

/*
 * pipelined out_vc_remain
 */
void DcProc::out_vc_remain_reg_process() {
	if (reset.read()) {
		for (int vo = 0; vo < RouterParameter::n_VCs; vo++) {
			out_vc_remain_reg[vo].write((int) RouterParameter::buffer_size);
		}
	} else {
		for (int vo = 0; vo < RouterParameter::n_VCs; vo++) {
			out_vc_remain_reg[vo].write(out_vc_remain[vo]);
		}
	}
}

/*
 * count_plus = out_vc_buffer_rd
 */
void DcProc::count_plus_process() {
	for (int vo = 0; vo < RouterParameter::n_VCs; vo++) {
		count_plus[vo].write(out_vc_buffer_rd[vo].read());
	}
}
