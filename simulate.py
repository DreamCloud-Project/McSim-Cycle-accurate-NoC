#! /usr/bin/env python2

import argparse
import os
import sys
import subprocess
import collections
import re

DEFAULT_ITERATIONS = 1
DEFAULT_APP = 'DC'
DEFAULT_MAPPING_STRATEGY = 'ZigZag'
DEFAULT_OUTPUT_FOLDER = '/OUTPUT_FILES'
DEFAULT_SCHEDULING_STRATEGY = 'prio'
DEFAULT_ROWS = 4
DEFAULT_COLS = 4
DEFAULT_FREQ = 1000000000

VCS = [1, 2, 4, 8, 16]
BUFFER_SIZES = [1, 2, 4, 8, 16]

FREQ_UNITS = {
 'GHz' : 1000000000,
 'MHz' : 1000000,
 'KHz' : 1000,
 'Hz'  : 1
}

APP_NAMES_TO_FILES = {
 'DC'  : '/apps/DEMO_CAR/DemoCar-PowerUp.amxmi',
}

MAPPINGS = ['MinComm', 'Static', 'ZigZag', 'Random']

class ValidateMapping(argparse.Action):
    def __call__(self, parser, args, values, option_string=None):
        mapping = values[0]
        if not mapping in MAPPINGS:
             raise ValueError('invalid mapping {s!r}'.format(s=mapping))
        mappingFile = None
        if mapping.startswith('Static'):
            if len(values) == 1:
                 raise ValueError('{s!r} mapping requires a file option'.format(s=mapping))
            mappingFile = values[1]
        randomfixedSeed = None
        if mapping == 'Weka':
            if len(values) == 1:
                 raise ValueError('{s!r} mapping requires a seed option'.format(s=mapping))
            randomfixedSeed = values[1]
        setattr(args, self.dest, [mapping, mappingFile, randomfixedSeed])

class ValidateFreq(argparse.Action):
    def invalidFreq(self, freq):
         raise ValueError('{s!r} is an invalid frequency. Correct format is a number followed by a unit among {units}'.format(s=freq, units=FREQ_UNITS.keys()))
    def __call__(self, parser, args, values, option_string=True):
        freq=values
        freqRegEx = re.compile('(\d+)(.*)')
        m = freqRegEx.search(freq)
        if m:
            try:
                value = int(m.groups()[0])
            except:
                self.invalidFreq(freq)
            try:
                unit = m.groups()[1]
            except:
                self.invalidFreq(freq)
            freqInHz = value * FREQ_UNITS[unit]
            if freqInHz > 1000000000:
                raise ValueError('{s!r} is an invalid frequency. Maximum supported value is 1GHz'.format(s=freq))
            setattr(args, self.dest, freqInHz)
        else:
            self.invalidFreq(freq)

def main():
    ''' Run the cycle accurate simulator '''
    # Configure parameters parser
    parser = argparse.ArgumentParser(description='Cycle accurate simulator runner script')
    parser.add_argument('-d', '--syntax_dependency', action='store_true', help='consider successive runnables in tasks call graph as dependent')
    appGroup = parser.add_mutually_exclusive_group()
    parser.add_argument('-bs', '--buffer_size', type=int, help='specify the number of slots in buffers (default is 16)', choices=BUFFER_SIZES)
    appGroup.add_argument('-da', '--def_application', help='specify the application to be simulated among the default ones', choices=['DC'])
    appGroup.add_argument('-ca', '--custom_application', help='specify a custom application file to be simulated')
    parser.add_argument('-f', '--freq', help='specify the frequency of cores in the NoC. Supported frequency units are Hz, KHz, MHz and GHz e.g 400MHz or 1GHz', action=ValidateFreq)
    appGroup.add_argument('-mf', '--modes_file', help='specify a modes switching file to be simulated')
    parser.add_argument('-m', '--mapping_strategy', help='specify the mapping strategy used to map runnables on cores and labels on memories. Valide strategies are ' + str(MAPPINGS), nargs="+",  action=ValidateMapping)
    parser.add_argument('-mw', '--micro_workload', action='store_true', help='simulate a micro workload of the application instead of the real application')
    parser.add_argument('-mww', '--micro_workload_width', type=int, help='the width of the simulated micro workload. To be used with -mw only')
    parser.add_argument('-mwh', '--micro_workload_height', type=int, help='the height of the simulated micro workload. To be used with -mw only')
    parser.add_argument('-np', '--no_periodicity', action='store_true', help='run periodic runnables only once')
    parser.add_argument('-nvc', '--nb_virtual_channels', type=int, help='specify the number of virtual channels (default is 8)', choices=VCS)
    parser.add_argument('-o', '--output_folder', help='specify the absolute path of the output folder where simulation results will be generated')
    parser.add_argument('-r', '--random', action='store_true', help='replace constant seed used to generate distributions by a random one based on current time')
    parser.add_argument('-s', '--scheduling_strategy', help='specify the scheduling strategy used by cores to choose the runnable to execute', choices=['prio'])
    parser.add_argument('-v',  '--verbose', action='store_true', help='enable verbose output')
    parser.add_argument('-x', '--rows', type=int, help='specify the number of rows in the NoC')
    parser.add_argument('-y', '--cols', type=int, help='specify the number of columns in the NoC')

    # Get parameters
    args = parser.parse_args()
    if args.def_application:
        app = os.path.dirname(os.path.realpath(__file__)) + APP_NAMES_TO_FILES[args.def_application]
    elif args.custom_application:
        app = args.custom_application
    else:
        app = os.path.dirname(os.path.realpath(__file__)) + APP_NAMES_TO_FILES['DC']
    mapping = DEFAULT_MAPPING_STRATEGY
    mappingFile = None
    mappingSeed = None
    if args.mapping_strategy:
        mapping = args.mapping_strategy[0]
        mappingFile = args.mapping_strategy[1]
        mappingSeed = args.mapping_strategy[2]
    out = os.path.dirname(os.path.realpath(__file__)) + DEFAULT_OUTPUT_FOLDER
    if args.output_folder:
        out = args.output_folder
    if not os.path.exists(out):
        os.makedirs(out)
    sched = DEFAULT_SCHEDULING_STRATEGY
    if args.scheduling_strategy:
        sched = args.scheduling_strategy
    freq = DEFAULT_FREQ
    if args.freq:
        freq = args.freq
    rows = DEFAULT_ROWS
    if args.rows:
        rows = args.rows
    cols = DEFAULT_COLS
    if args.cols:
        cols = args.cols

    # Add systemc lib to LD_LIBRARY_PATH
    my_env = os.environ.copy()
    sc_home = my_env.get('SYSTEMC_HOME', '')
    if not sc_home:
        sys.stderr.write('You must define the SYSTEMC_HOME variable to use this script')
        sys.exit(-1)
    for file in os.listdir(sc_home):
        if 'lib-' in file:
            my_env['LD_LIBRARY_PATH'] = my_env.get('LD_LIBRARY_PATH', '') + ':' + sc_home + '/' + file
            break
    my_env['SC_COPYRIGHT_MESSAGE'] = 'DISABLE'

    # Run the simulation
    cmd = [os.path.dirname(os.path.realpath(__file__)) + '/obj/mcsim-ca-noc', '-mapping', mapping,]
    if mappingFile:
        cmd.append(mappingFile)
    if mappingSeed:
        cmd.append(mappingSeed)
    cmd.extend(('-platform', 'dc', '-outputFolder', out, '-scheduling', sched, '-dimx', str(rows), '-dimy', str(cols), '-freq', str(freq)))
    if args.modes_file:
        cmd.extend(('-f', args.modes_file))
    else:
        cmd.extend(('-app', app))
    if args.syntax_dependency:
         cmd.append('-d')
    if args.no_periodicity:
         cmd.append('-np')
    if args.nb_virtual_channels:
         cmd.append('-nvc')
         cmd.append(str(args.nb_virtual_channels))
    if args.buffer_size:
         cmd.append('-bsize')
         cmd.append(str(args.buffer_size))
    if args.micro_workload:
         cmd.append('-mw')
         if args.micro_workload_width:
             cmd.append('-mww')
             cmd.append(str(args.micro_workload_width))
         if args.micro_workload_height:
             cmd.append('-mwh')
             cmd.append(str(args.micro_workload_height))
    if not args.random:
         cmd.extend(('-seed', "42"))
    if args.verbose:
        cmdStr = ''
        for s in cmd:
            cmdStr = cmdStr + ' ' + s
        print (cmdStr)
    sim = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, env=my_env)
    stdout, stderr = sim.communicate()
    outFile = open(out + '/' + 'OUTPUT_Execution_Report.log', 'w')
    outFile.write(stdout)
    outFile.close()
    print stdout,
    if sim.returncode != 0:
        if not args.verbose:
            print stderr,
        print ('simulation FAILED')
        exit(-1)

    # Run the energy estimator module
    cmd = [os.path.dirname(os.path.realpath(__file__)) + '/src/energy_estimator/Power_Multiproc.py', out]
    nrj = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    stdout, stderr = nrj.communicate()
    if nrj.wait() != 0:
        print stderr,
        print ('Energy estimation FAILED')
        exit(-1)
    outFile = open(out + '/' + 'OUTPUT_Energy.log', 'w')
    outFile.write(stdout)
    outFile.close()

# This script runs the main
if __name__ == "__main__":
    main()
