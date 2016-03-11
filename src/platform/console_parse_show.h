/*
 * console_parse_show.h
 *
 *  Created on: May 28, 2010
 *      Author: Anh Tran
 *
 *  Functions for parsing command line and showing information to console
 */

#ifndef CONSOLE_PARSE_SHOW_H_
#define CONSOLE_PARSE_SHOW_H_

#include <iostream>
#include <sstream>
#include "common_parameters.h"
#include "router/router_parameters.h"
#include "proc/proc_parameters.h"
#include "proc/synthetic/synthetic_parameters.h"
#include "proc/embedded/embedded_parameters.h"
#include "proc/dreamcloud/dc_parameters.h"

#include "global_variables.h"
#include "timing_parameters.h"

extern ofstream log_file;

// Command line parser
void command_line_parser(int argc, char *argv[]);

// show help
void show_help();

void show_processor_settings();

// show synthetic network settings
void show_synthetic_settings();

void show_embedded_settings();

void show_router_settings();

#endif /* CONSOLE_PARSE_SHOW_H_ */
