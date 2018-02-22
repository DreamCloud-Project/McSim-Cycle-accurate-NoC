/*
 * embedded_parameters.h
 *
 *  Created on: Aug 17, 2011
 *      Author: anhttran
 */

#ifndef DC_PARAMETERS_H_
#define DC_PARAMETERS_H_

// Uncomment to have debug info
//#define DEBUG_SIMU 1


#include <map>
#include "commons/mapping_heuristic/dcMappingHeuristicI.hxx"

typedef map<string, dreamcloud::platform_sclib::dcMappingHeuristicI::dcMappingLocation> DCLabelsMapping;

struct DCParameters{
	static DCLabelsMapping labelsMap; // global table of labels mapping
	static string mapping;
	static string mappingFile;
        static int mappingSeed;
	static string appXml;
	static string modeFile;
	static string outputFolder;
	static string scheduling;
	static unsigned int iterations;
	static bool seqDep;
	static bool useMicroworkload;
	static unsigned int microworkloadWidth;
	static unsigned int microworkloadHeight;
	static bool generateWaveforms;
	static bool dontHandlePeriodic;
	static unsigned long int coresFrequencyInHz;
	static int instructionsPerCycle;
};


#endif /* DC_PARAMETERS_H_ */
