#!/usr/bin/python

###############################################################################################################
# Launch this script by ./Power_Multiproc.py.
# The script computes the energy of the executed application
# Author : Roman Ursu
# Date : 30.11.2015
###############################################################################################################

import re
import os
import subprocess
import random
import time
import argparse
import sys
import array

# Get arguments
parser = argparse.ArgumentParser(description='Estimate energy consumption')
parser.add_argument('inputFolder', metavar='S', type=str, nargs='+',
                   help='The folder where to look for input files for the energy estimation')
args = parser.parse_args()

#Configuration variables
config_freq = 0 #in MHz
inst_complex_min = 0 #complexity of the computation instruction
inst_complex_max = 0 #complexity of the computation instruction

#*******************************************************************************************************************************************************************
#Calibration of NoC power characteristics
#For a router, the number of input ports is kept constnt and is equal to 5. The number of Virtual Channels per input is the unique variable of the router model.
#This variable is set in the Configuration.txt file
#*******************************************************************************************************************************************************************


# Define some classes

class Energy:
    def __init__(self):
        #Total Energy of the platform
        self.E_total_NoC = 0
        self.E_static_NoC = 0
        self.E_dynamic_NoC = 0
        self.E_static_cores=0
        self.E_dynamic_cores=0
        self.E_static_platform=0
        self.E_dynamic_platform=0
        #Energy of Receiving/Sending buffers
        self.Dyn_energy_RB_SB = 0
        self.Stat_energy_RB_SB = 0
        self.Tot_energy_RB_SB = 0
        #Energy due to Labels
        self.Tot_energy_labels = 0
        self.Stat_energy_labels = 0
        self.Dyn_energy_labels = 0
        self.Dyn_energy_label_set = 0
        self.Stat_energy_label_set = 0
        self.Tot_energy_label_set = 0
        #Energy of REB/WRB/PRB buffers
        self.Tot_energy_WRB_REB = 0
        self.Dyn_energy_WRB_REB = 0
        self.Stat_energy_WRB = 0
        self.Stat_energy_REB = 0
        self.Stat_energy_PRB = 0
        #Energy other
        self.Tot_energy_preemptions = 0
        self.Tot_energy_exec = 0
        self.Tot_energy = 0
        self.Tot_energy_static = 0
        self.Tot_energy_dynamic = 0
        self.Tot_energy_cores = 0
        self.Tot_energy_cores_static = 0
        self.Tot_energy_cores_dynamic = 0
        self.Tot_energy_cpu_idle = 0
        self.Power_dynamic_mean = 0

    def print_all(self):
        print "Energy : "
        print " Total energy NoC (micro J): "; print self.E_total_NoC
        print " Static energy NoC (micro J): "; print self.E_static_NoC
        print " Dynamic energy NoC (micro J): "; print self.E_dynamic_NoC
        #print " Mean dynamic power : "; print self.Power_dynamic_mean


class Energy_Characteristics_Router:
    def __init__(self):
        #Arbiter
        self.arbiter_static_0 = 1.57*1E-8
        self.arbiter_static_1 = 1.489*1E-7
        self.arbiter_dynamic = 2*1E-12 #Power per flit per input per VC
        #Crossbar
        self.crossbar_static = 6.29*1E-6
        self.crossbar_dynamic = 1.88*1E-12 #Power per flit per input
        #Buffer
        self.buffer_static_0 = 8.87*1E-5
        self.buffer_static_1 = -1.0*1E-4
        self.buffer_static_2 = 6.979*1E-5
        #Power per flit per input per VC
        self.buffer_dynamic_0 = 6.63*1E-12;
        self.buffer_dynamic_1 = 1.72*1E-11;
        self.buffer_dynamic_2 = 5.16*1E-12;
        #Links
        #per router
        self.links_static_0 = 1.43*1E-6
        self.links_static_1 = 4.19*1E-7
        self.links_static_2 = 1.14*1E-5
        #per router per flit per input
        self.links_dynamic_0 = 6.63*1E-12
        self.links_dynamic_1 = 1.72*1E-11
        self.links_dynamic_2 = 5.16*1E-12


class Buffers_Characteristics:
    def __init__(self):
        # Parameters buffers
        self.Nb_banks_buffers = 0
        self.size_WRB = 0 # Size in kB of WRB
        self.size_REB = 0 # Size in kB of REB
        self.size_PRB = 0 # Size in kB of PRB (Periodic Runnables Buffer)
        self.Dynamic_energy_read_port_WRB = 0 #nJ
        self.Dynamic_energy_read_port_REB = 0 #nJ
        self.Dynamic_energy_read_port_PRB = 0 #nJ

        self.Static_power_bank_WRB = 0 #Watts
        self.Static_power_bank_REB = 0 #Watts
        self.Static_power_bank_PRB = 0 #Watts

        self.size_SB = 0 #Size in Bytes of SB
        self.size_RB = 0 #Size in Bytes of RB
        self.Dynamic_energy_read_port_RB_SB = 0 #nJ
        self.Static_power_bank_RB_SB = 0 #Watts

    def print_all(self):
        print "Buffers_Characteristics : "
        print " Number of banks per buffer : "; print self.Nb_banks_buffers
        print " Size of the WRB buffer (kB) : "; print self.size_WRB
        print " Size of the REB buffer (kB) : "; print self.size_REB
        print " Size of the PRB buffer (kB) : "; print self.size_PRB
        print " Dynamic energy read port WRB (nJ) : "; print self.Dynamic_energy_read_port_WRB
        print " Dynamic energy read port REB (nJ) : "; print self.Dynamic_energy_read_port_REB
        print " Dynamic energy read port PRB (nJ) : "; print self.Dynamic_energy_read_port_PRB
        print " Static power per bank WRB (W) : "; print self.Static_power_bank_WRB
        print " Static power per bank REB (W) : "; print self.Static_power_bank_REB
        print " Static power per bank PRB (W) : "; print self.Static_power_bank_PRB
        print " Size Sending Buffer (B) : "; print self.size_SB
        print " Size Receiving Buffer (B) : "; print self.size_RB
        print " Dynamic energy read port RB SB (nJ) : "; print self.Dynamic_energy_read_port_RB_SB
        print " Static power bank RB SB (W) : "; print self.Static_power_bank_RB_SB
        
class Labels_Characteristics:
    def __init__(self):
        # Parameters labels
        self.size_Labels = 0 # Size in MB of local RAM
        self.Nb_banks_labels = 0
        self.Nb_bits_read_out = 0 #same value for labels and buffers
        #We assume in each case that the number of read ports equals the number of banks (the same holds for the number of write ports)
        self.Dynamic_energy_read_port_label = 0 #nJ
        self.Static_power_bank_label = 0 #Watts
        self.Percentage_refresh_of_leakage = 0

    def print_all(self):
        print "Labels Characteristics : "
        print " Size (MB) :  "; print self.size_Labels
        print " Number of banks : "; print self.Nb_banks_labels
        print " Number of bits read out : "; print self.Nb_bits_read_out
        print " Dynamic energy read port label (nJ) : "; print self.Dynamic_energy_read_port_label
        print " Static power per bank (W) : "; print self.Static_power_bank_label
        print " Refresh power as percentage of the leackage: "; print self.Percentage_refresh_of_leakage


class cpu:
    def __init__(self):
        # Parameters labels
        self.name = ""
        self.technology = 0 #in nm
        self.temperature = 0 #in K
        self.stat_pow = 0 #Watts
        self.empty_pow = 0 #Watts
        self.table_complex_pow = [] #dynamic power of instructions according to their complexity (from 10 to 130% of core static power)

    def init_table_complex_pow(self,config_freq,clk_freq):
        self.technology = 28
        self.temperature = 310
        (self.table_complex_pow).append((clk_freq*1e+3/config_freq)*self.stat_pow*0.1) #W
        (self.table_complex_pow).append((clk_freq*1e+3/config_freq)*self.stat_pow*0.3) #W
        (self.table_complex_pow).append((clk_freq*1e+3/config_freq)*self.stat_pow*0.5) #W
        (self.table_complex_pow).append((clk_freq*1e+3/config_freq)*self.stat_pow*0.7) #W
        (self.table_complex_pow).append((clk_freq*1e+3/config_freq)*self.stat_pow*0.9) #W
        (self.table_complex_pow).append((clk_freq*1e+3/config_freq)*self.stat_pow*1.1) #W
        (self.table_complex_pow).append((clk_freq*1e+3/config_freq)*self.stat_pow*1.3) #W

    def print_all(self):
        print "CPU Characteristics : "
        print " name :  "; print self.name
        print " technology (nm) : "; print self.technology
        print " temperature (K) : "; print self.temperature
        print " stat_pow (W) : "; print self.stat_pow
        print " empty_pow (W) : "; print self.empty_pow
        print " Table compexity-power : (W) "
        print "    "; print self.table_complex_pow(0)
        print "    "; print self.table_complex_pow(1)
        print "    "; print self.table_complex_pow(2)
        print "    "; print self.table_complex_pow(3)
        print "    "; print self.table_complex_pow(4)
        print "    "; print self.table_complex_pow(5)
        print "    "; print self.table_complex_pow(6)


class Runnable:
    def __init__(self):
        # Parameters labels
        self.index = 0
        self.name = ""
        self.id = ""
        self.Nb_instructions = 0
        self.Nb_label_accesses = 0
        self.Nb_packets = 0 #Theoretical number of total packets exchanged by the Runnable if the all Label acccesses were to be performed through the NoC
        self.Nb_bytes_per_instruction = 0 #For storing runnables in WRB and REB during premptions: The size of a runnable is estimated as : nb_instr*Nb_bytes_per_instruction+Nb_bytes_offset
        self.Nb_bytes_offset = 0

class Application_Characteristics:
    def __init__(self):
        self.Nb_runnables = 0
        self.Nb_mult_occ_runnables = 0 #Number of all accurrences of runnables (several runnables occur multiple times)
        self.Time_ins = 0 # Total execution time of the instructions (in the number of cycles)
        self.total_flits = 0
        self.packets = 0 # Total number of packets (generated by the label accesses)
        self.runs = []

    def print_all(self):
        print "Application Characteristics : "
        print " Nb runnables :  "; print self.Nb_runnables
        print " Number of multiple occurences of runnables : "; print self.Nb_mult_occ_runnables
        print " Time of computation instructions (nb of cycles) : "; print self.Time_ins
        print " Content of Runnables_Name_Idx"
        l = len(self.Runnables_Name_Idx)
        print " Size of Runnables_Name_Idx : "; print l
        #for i in range(l):
            #print self.Runnables_Name_Idx[i]
        print " Content of Run_Idx"
        l = len(self.Run_Idx)
        print " Size of Run_Idx : "; print l
        #for i in range(l):
            #print self.Run_Idx[i].Runn_Name
            #print self.Run_Idx[i].Nb_instructions
            #print self.Run_Idx[i].Nb_label_accesses
            #print self.Run_Idx[i].Nb_packets #Theoretical number of total packets exchanged by the Runnable if the all Label acccesses were to be performed through the NoC
            #print self.Run_Idx[i].Nb_bytes_per_instruction #For storing runnables in WRB and REB during premptions: The size of a runnable is estimated as : nb_instr*Nb_bytes_per_instruction+Nb_bytes_offset
            #print self.Run_Idx[i].Nb_bytes_offset
        print " Total flits : "; print self.total_flits
        print " Total nb of packets generated by label accesses : "; print self.packets


class Platform_Characteristics:
    def __init__(self):
        # Parameters buffers
        self.cpu_name = ""
        self.clock = 0
        self.Columns = 0
        self.Rows = 0
        self.Number_in_out_Router = 5 #Number of input/output Links per Router (we assume the same number of inputs and outputs)self.size_REB = 0 # Size in kB of REB
        self.N_5 = 0 #Number of routers with 5 inputs/outputs
        self.N_4 = 0 #Number of routers with 4 inputs/outputs
        self.N_3 = 0 #Number of routers with 3 inputs/outputs
        self.N_2 = 0 #Number of routers with 2 inputs/outputs
        self.N_processors = 0
        self.N_channels = 0
        self.Number_VC_per_input = 0
        self.N_flt_pack = 32 # Number of flits in packet
        self.N_bytes_flit = 1 # Nb of bytes per flit. A flit equals a phit. A phit is 8 bits wide (width of the buss).
        self.N_clk_flt_link = 0 # Average number of clk cycles for a flit to traverse a Link
        self.N_clk_flt_router = 0 # Average number of clk cycles for a flit to traverse a Router
        self.Size_Dist_vector = 0

    def print_all(self):
        print "Platform Characteristics : "
        print " CPU :  "; print self.cpu_name
        print " Columns :  "; print self.Columns
        print " Rows : "; print self.Rows
        print " Nb processors : "; print self.N_processors
        print " Nb channels : "; print self.N_channels
        print " Nb of Virtual Channels per input : "; print self.Number_VC_per_input
        print " Number of flits in packet : "; print self.N_flt_pack
        print " Nb of bytes per flit : "; print self.N_bytes_flit
        print " Number of input/output links per Router : "; print self.Number_in_out_Router
        print " Average number of clk cycles for a flit to traverse a Link : "; print self.N_clk_flt_link
        print " Average number of clk cycles for a flit to traverse a Router : "; print self.N_clk_flt_router
        print " Size of the distance vector :  "; print self.Size_Dist_vector
        print " N_5 : "; print self.N_5
        print " N_4 : "; print self.N_4
        print " N_3 : "; print self.N_3
        print " N_2 : "; print self.N_2


class Execution_Characteristics:
    def __init__(self):
        self.exec_time = 0 # Execution time of the application (ns)
        self.Dist_vector= [] #Vector of distances : Dis[i] is the number of packets traveling the distance i. 'H' means unsigned short (in C)
        self.Duration_packet = 0
        self.Duration_flit_hop = 0
        self.Mean_duration_flit = 0
        self.Mean_duration_packet = 0
        self.N_clk_flt = 1 # Average number of clock (NoC clock) cycles per flit
        self.seed_mode = 0
        self.runIds = []
        self.N_mesg = 0 # Overall Number of packets
        self.S = 0 # Sum of the lengths of all routes
        self.Complexity_mean = 0
        self.remote_flits = 0


    def print_all(self):
        print "Execution_Characteristics : "
        print " Execution time (ns):  "; print self.exec_time
        print " Overall duration of all packets (ns) :  "; print self.Duration_packet
        print " Overall duration of all flits travelling a hop (ns):  "; print self.Duration_flit_hop
        print " Average time for a flit to realise a hop (ns):  "; print self.Mean_duration_flit
        print " Average time for a packet to come to its destination (ns):  "; print self.Mean_duration_packet
        print " Average number of clock (NoC clock) cycles per flit:  "; print self.N_clk_flt
        print " Seed mode (1 : variable seed (tyme(NULL)); 0 : constant seed) : "; print self.seed_mode
        print " Number of messages : "; print self.N_mesg
        print " Sum of the lengths of all routes : "; print self.S
        print " Content of Runnables_ID_Name"
        l = len(self.Runnables_ID_Name)
        print " Size of Runnables_ID_Name : "; print l
        #for i in range(l):
            #print (self.Runnables_ID_Name)[i]
        print " Complexity mean : "; print self.Complexity_mean
        print " Remote flits : "; print self.remote_flits



class Tools:
    def Check_presence(self,Runnables_ID_Name, Runnable_name,i): # Check the presence of Runnable_name in Runnables_ID_Name for indexes less than i
        present = False
        count=0
        while ((count < i)&(not present)):
            present = (Runnables_ID_Name[count]==Runnable_name)
            count = count + 1
        return present

    def getRun(self, runs, id):
        for run in runs:
            if run.id == id:
                return run
        return None

    # Inst_length is the number of cycles; clock is the clock frequency in GHz
    def get_Energy_Instruction(self,library, lib, Inst_length,Inst_complex,clock):
        Power_dynamic=0
        Power_dynamic = ((library[lib].table_complex_pow)[Inst_complex-1])
        #print "Power_dynamic : "; print Power_dynamic
        return (((Inst_length)/(clock*1e+9))*Power_dynamic)




Tool = Tools()
Application_C = Application_Characteristics()
Platform_C = Platform_Characteristics()
Labels_C = Labels_Characteristics()
Buffers_C = Buffers_Characteristics()
Execution_C = Execution_Characteristics()
Energy_CR = Energy_Characteristics_Router()
Energy = Energy()


#*************************************************************************************************************//
#*                                                                                                           *//
#*                                           Read Parameters 1                                               *//
#*                                                                                                           *//
#*************************************************************************************************************//

# Read clock frequency and execution time of the application
path_inputFolder = args.inputFolder[0]
file_path = path_inputFolder+'/OUTPUT_Execution_Report.log'
f_file = open(file_path,'r')
for line in f_file:
    cleanLine = " ".join(line.split())
    execTimeRegEx = re.compile('Execution time of the application : (\d+) ')
    m = execTimeRegEx.search(cleanLine)
    if m:
        try:
            Execution_C.exec_time = int(m.groups()[0])
        except ValueError, e:
            print "Error parsing execution time, I got " +  m.groups()[0] + " instead of a number..."
            sys.exit()

    freqRegEx = re.compile('Cores operating frequency : (\d+) ')
    m = freqRegEx.search(cleanLine)
    if m:
        try:
            Platform_C.clock = float(m.groups()[0])
        except ValueError, e:
            print "Error parsing Rows, I got " +  m.groups()[0]  + " instead of a number..."

    rowsRegEx = re.compile('Number of rows in platform : (\d+)')
    m = rowsRegEx.search(cleanLine)
    if m:
        try:
            Platform_C.Rows = int(m.groups()[0])
        except ValueError, e:
            print "Error parsing execution time, I got " +  m.groups()[0]  + " instead of a number..."

    colsRegEx = re.compile('Number of columns in platform : (\d+)')
    m = colsRegEx.search(cleanLine)
    if m:
        try:
            Platform_C.Columns = int(m.groups()[0])
        except ValueError, e:
            print "Error parsing Columns, I got " +  Platform_C.Rows  + " instead of a number..."

f_file.close

Platform_C.Size_Dist_vector = (Platform_C.Rows-1)+(Platform_C.Columns-1)+1;
Platform_C.N_processors = Platform_C.Rows*Platform_C.Columns;
Platform_C.N_channels = 5*(Platform_C.Rows-2)*(Platform_C.Columns-2)+4*3+4*(2*Platform_C.Columns+2*(Platform_C.Rows-2)-4); # Number of channels on the platform (a channel is a link from one router to a neighbour)

#Determine the number of routers with different number of inputs/outputs : Several routers have more input/output ports than teh others depending on wether the router is in the modle or at the edge of the platform
if((Platform_C.Columns>=2)|(Platform_C.Rows >= 2)):
    Platform_C.N_5 = (Platform_C.Rows-2)*(Platform_C.Columns-2)
    Platform_C.N_4 = 2*(Platform_C.Columns+Platform_C.Rows-4)
    Platform_C.N_3 = 4
    Platform_C.N_2 = 0
else:
    if((Platform_C.Columns>=2)&(Platform_C.Rows<2)):
        Platform_C.N_5 = 0
        Platform_C.N_4 = 0
        Platform_C.N_3 = Platform_C.Columns-2
        Platform_C.N_2 = 2
    if((Platform_C.Rows>=2)&(Platform_C.Columns<2)):
        Platform_C.N_5 = 0
        Platform_C.N_4 = 0
        Platform_C.N_3 = Platform_C.Rows-2
        Platform_C.N_2 = 2
    if((Platform_C.Rows==1)&(Platform_C.Columns==1)):
        Platform_C.N_5 = 0
        Platform_C.N_4 = 0
        Platform_C.N_3 = 0
        Platform_C.N_2 = 0

# Initialisation of the vector of distances//
for i in range(Platform_C.Size_Dist_vector):
    Execution_C.Dist_vector.append(0)

#*************************************************************************************************************//
#*                                                                                                           *//
#*                                           Read Configuration                                              *//
#*                                                                                                           *//
#*************************************************************************************************************//
cpu_ARM=cpu()
cpu_ARM.name = "ARM_A15"

cpu_ALPHA=cpu()
cpu_ALPHA.name = "ALPHA_21364"


file_path = os.path.dirname(os.path.realpath(__file__)) + '/Configuration_Parameters/Configuration.txt'
f_file = open(file_path,'r')
for line in f_file:
    # searching for exec_time in: [HOST_Frontend:master:(1) exec_time] [msg_test/INFO] Master completed.
    # searchObj = re.search("\[HOST_Frontend\:master\:\(1\) (\S+)\] \[msg_test\/INFO\] Master completed", line)
    searchObj = re.search("Configuration frequency\(MHz\)\s*:\s*(\S+)", line)
    if searchObj:
        try:
            config_freq = int(searchObj.group(1))
        except ValueError, e:
            print "Error parsing configuration frequency"

    searchObj = re.search("Static power of the ALPHA_cpu\(W\)\s*:\s*(\S+)", line)
    if searchObj:
        try:
            cpu_ALPHA.stat_pow = float(searchObj.group(1))
            cpu_ALPHA.init_table_complex_pow(config_freq,Platform_C.clock)
        except ValueError, e:
            print "Error parsing Power Static"

    searchObj = re.search("Dynamic power of an empty ALPHA_cpu\(W\)\s*:\s*(\S+)", line)
    if searchObj:
        try:
            cpu_ALPHA.empty_pow = float(searchObj.group(1))
        except ValueError, e:
            print "Error parsing Power Empty"


    searchObj = re.search("Static power of the ARM_cpu\(W\)\s*:\s*(\S+)", line)
    if searchObj:
        try:
            cpu_ARM.stat_pow = float(searchObj.group(1))
            cpu_ARM.init_table_complex_pow(config_freq,Platform_C.clock)
        except ValueError, e:
            print "Error parsing Power Static"

    searchObj = re.search("Dynamic power of an empty ARM_cpu\(W\)\s*:\s*(\S+)", line)
    if searchObj:
        try:
            cpu_ARM.empty_pow = float(searchObj.group(1))
        except ValueError, e:
            print "Error parsing Power Empty"


    searchObj = re.search("Number of VC per input\s*:\s*(\S+)", line)
    if searchObj:
        try:
             Platform_C.Number_VC_per_input = int(searchObj.group(1))
        except ValueError, e:
            print "Error parsing Number of VC per input"


    searchObj = re.search("Cpu architecture\s*:\s*(\S+)", line)
    if searchObj:
        try:
             Platform_C.cpu_name = str(searchObj.group(1))
        except ValueError, e:
            print "Error parsing CPU architecture"

    searchObj = re.search("Computation instructions complexity interval\s*:\s*(\S+)\s*(\S+)", line)
    if searchObj:
        try:
             inst_complex_min = int(searchObj.group(1))
             inst_complex_max = int(searchObj.group(2))
        except ValueError, e:
            print "Error parsing CPU architecture"

    searchObj = re.search("Computation complexity selection mode\s*:\s*(\S+)", line)
    if searchObj:
        try:
             Execution_C.seed_mode = int(searchObj.group(1))
        except ValueError, e:
            print "Error parsing seed mode"

f_file.close

#set CPU architecture
#library index
lib = 0
if(Platform_C.cpu_name=="ARM"):
    lib = 0

if(Platform_C.cpu_name=="ALP"):
    lib = 1

#print lib

#set seed
if(Execution_C.seed_mode==1):
    random.seed(time.clock())
    #print "var seed"

if(Execution_C.seed_mode==0):
    random.seed(1942) #set fixed seed
    #print "fix seed"

#Define and declare a hardware library
#library is an arry of pointers to (structs cpu)
library=list()
library.append(cpu_ARM)
library.append(cpu_ALPHA)

#print cpu_ARM.stat_pow
#print cpu_ALPHA.stat_pow

Power_static = library[lib].stat_pow
#print Power_static



#*************************************************************************************************************//
#*                                                                                                           *//
#*                                           Read Parameters 2                                               *//
#*                                                                                                           *//
#*************************************************************************************************************//
# Read buffer sizes and Energy parameters

file_path = os.path.dirname(os.path.realpath(__file__)) +  '/Configuration_Parameters/Parameters_2.txt'
f_file = open(file_path,'r')
for line in f_file:
    # searching for exec_time in: [HOST_Frontend:master:(1) exec_time] [msg_test/INFO] Master completed.
    # searchObj = re.search("\[HOST_Frontend\:master\:\(1\) (\S+)\] \[msg_test\/INFO\] Master completed", line)
    searchObj = re.search("Size of local Label set\(MB\)\s*:\s*(\S+)", line)
    if searchObj:
        try:
            Labels_C.size_Labels = int(searchObj.group(1))
            #print "*******************" ; print Labels_c.size_Labels
        except ValueError, e:
            print "Error parsing size_Labelsy"

    searchObj = re.search("Number of banks\(labels\)\s*:\s*(\S+)", line)
    if searchObj:
        try:
            Labels_C.Nb_banks_labels = int(searchObj.group(1))
        except ValueError, e:
            print "Error parsing Nb_banks_labels"

    searchObj = re.search("Number of bits read out\s*:\s*(\S+)", line)
    if searchObj:
        try:
            Labels_C.Nb_bits_read_out = int(searchObj.group(1))
        except ValueError, e:
            print "Error parsing Nb_bits_read_out"


    searchObj = re.search("Dynamic energy per read port label set\(nJ\)\s*:\s*(\S+)", line)
    if searchObj:
        try:
            Labels_C.Dynamic_energy_read_port_label = float(searchObj.group(1)) # nJ per read port
        except ValueError, e:
            print "Error parsing Dynamic_energy_read_port_label"

    searchObj = re.search("Standby leakage power per bank\(W\)\s*:\s*(\S+)", line)
    if searchObj:
        try:
            Labels_C.Static_power_bank_label = float(searchObj.group(1))
        except ValueError, e:
            print "Error parsing Static_power_bank_label"

    searchObj = re.search("Refresh power \(percentage of leakage\)\s*:\s*(\S+)", line)
    if searchObj:
        try:
             Labels_C.Percentage_refresh_of_leakage = float(searchObj.group(1))
        except ValueError, e:
            print "Error parsing Percentage_refresh_of_leakage"

    searchObj = re.search("Size of WRB\(kB\)\s*:\s*(\S+)", line)
    if searchObj:
        try:
             Buffers_C.size_WRB = int(searchObj.group(1))
        except ValueError, e:
            print "Error parsing size_WRB"

    searchObj = re.search("Size of REB\(kB\)\s*:\s*(\S+)", line)
    if searchObj:
        try:
             Buffers_C.size_REB = int(searchObj.group(1))
        except ValueError, e:
            print "Error parsing size_REB"

    searchObj = re.search("Size of PRB\(kB\)\s*:\s*(\S+)", line)
    if searchObj:
        try:
             Buffers_C.size_PRB = int(searchObj.group(1))
        except ValueError, e:
            print "Error parsing size_PRB"

    searchObj = re.search("Number of banks buffers\s*:\s*(\S+)", line)
    if searchObj:
        try:
             Buffers_C.Nb_banks_buffers = int(searchObj.group(1))
        except ValueError, e:
            print "Error parsing Nb_banks_buffers"

    searchObj = re.search("Dynamic energy per read port WRB\(nJ\)\s*:\s*(\S+)", line)
    if searchObj:
        try:
             Buffers_C.Dynamic_energy_read_port_WRB = float(searchObj.group(1)) #nJ
        except ValueError, e:
            print "Error parsing Dynamic_energy_read_port_WRB"

    searchObj = re.search("Dynamic energy per read port REB\(nJ\)\s*:\s*(\S+)", line)
    if searchObj:
        try:
             Buffers_C.Dynamic_energy_read_port_REB = float(searchObj.group(1)) #nJ
        except ValueError, e:
            print "Error parsing Dynamic_energy_read_port_REB"

    searchObj = re.search("Dynamic energy per read port PRB\(nJ\)\s*:\s*(\S+)", line)
    if searchObj:
        try:
             Buffers_C.Dynamic_energy_read_port_PRB = float(searchObj.group(1)) #nJ
        except ValueError, e:
            print "Error parsing Dynamic_energy_read_port_PRB"

    searchObj = re.search("Standby leakage power per bank WRB\(W\)\s*:\s*(\S+)", line)
    if searchObj:
        try:
             Buffers_C.Static_power_bank_WRB = float(searchObj.group(1)) #Watts
        except ValueError, e:
            print "Error parsing Static_power_bank_WRB"


    searchObj = re.search("Standby leakage power per bank REB\(W\)\s*:\s*(\S+)", line)
    if searchObj:
        try:
             Buffers_C.Static_power_bank_REB = float(searchObj.group(1)) #Watts
        except ValueError, e:
            print "Error parsing Static_power_bank_REB"

    searchObj = re.search("Standby leakage power per bank PRB\(W\)\s*:\s*(\S+)", line)
    if searchObj:
        try:
             Buffers_C.Static_power_bank_PRB = float(searchObj.group(1)) #Watts
        except ValueError, e:
            print "Error parsing Static_power_bank_PRB"

    searchObj = re.search("Size of SB\(B\)\s*:\s*(\S+)", line)
    if searchObj:
        try:
             Buffers_C.size_SB = int(searchObj.group(1))
        except ValueError, e:
            print "Error parsing size_SB"

    searchObj = re.search("Size of RB\(B\)\s*:\s*(\S+)", line)
    if searchObj:
        try:
             Buffers_C.size_RB = int(searchObj.group(1))
        except ValueError, e:
            print "Error parsing  size_RB"


    searchObj = re.search("Dynamic energy per read port RB\(nJ\)\s*:\s*(\S+)", line)
    if searchObj:
        try:
             Buffers_C.Dynamic_energy_read_port_RB_SB = float(searchObj.group(1))
        except ValueError, e:
            print "Error parsing Dynamic_energy_read_port_RB_SB"

    searchObj = re.search("Standby leakage power per bank RB\(W\)\s*:\s*(\S+)", line)
    if searchObj:
        try:
             Buffers_C.Static_power_bank_RB_SB = float(searchObj.group(1)) #Watts
        except ValueError, e:
            print "Error parsing Static_power_bank_RB_SB"

    searchObj = re.search("Size for storing instruction\(bytes\)\s*:\s*(\S+)", line)
    if searchObj:
        try:
             Buffers_C.Nb_bytes_per_instruction = int(searchObj.group(1))
        except ValueError, e:
            print "Error parsing Nb_bytes_per_instruction"


    searchObj = re.search("Size for storing instructions offset\(bytes\)\s*:\s*(\S+)", line)
    if searchObj:
        try:
             Buffers_C.Nb_bytes_offset = int(searchObj.group(1))
        except ValueError, e:
            print "Error parsing Nb_bytes_offset"

f_file.close

# Read Number of Runnables
file_path = path_inputFolder+'/OUTPUT_Execution_Report.log'
f_file = open(file_path,'r')
for line in f_file:
    # searching for exec_time in: [HOST_Frontend:master:(1) exec_time] [msg_test/INFO] Master completed.
    # searchObj = re.search("\[HOST_Frontend\:master\:\(1\) (\S+)\] \[msg_test\/INFO\] Master completed", line)
    searchObj = re.search("Number of runnables\s*:\s*(\S+)", line)
    if searchObj:
        try:
            Application_C.Nb_mult_occ_runnables = int(searchObj.group(1))
        except ValueError, e:
            print "Error parsing Nb_mult_occ_runnables"

f_file.close

if (Application_C.Nb_mult_occ_runnables < 0)|(Application_C.Nb_mult_occ_runnables > 1e+6):
    print "Energy estimator has more than 1 billion of runnables is it normal ?"

#*************************************************************************************************************//
#*                                                                                                           *//
#*                                Determine the list of runnables                                            *//
#*                                                                                                           *//
#*************************************************************************************************************//

# List of executed runnable instances
file_path = path_inputFolder + '/OUTPUT_Runnable_Traces.csv'
f_file = open(file_path,'r')
f_file.readline() #Skip the first line
for line in f_file:
   splitLine=line.split(',')
   id = splitLine[2]
   Execution_C.runIds.append(id)

f_file.close

# List of uniq runnables
uniqRunIds = set(Execution_C.runIds)
for runId in uniqRunIds:
    r = Runnable()
    r.id = runId
    Application_C.runs.append(r)
Application_C.Nb_runnables = len(uniqRunIds)


#*************************************************************************************************************//
#*                                                                                                           *//
#*                                          Describe the runnables                                           *//
#*                                                                                                           *//
#*************************************************************************************************************//

file_path = path_inputFolder+'/Runnables.txt'
f_file = open(file_path,'r')
for line in f_file:
    splitLine = line.split(' ')
    id = splitLine[0]
    name = splitLine[1]
    nbInsts = int(splitLine[2])
    nbLabels = int(splitLine[3])
    nbPackets = int(splitLine[4])
    run = Tool.getRun(Application_C.runs, id)
    run.Nb_instructions =  nbInsts
    run.Nb_label_accesses =  nbLabels
    run.Nb_packets =  nbPackets

f_file.close


#*************************************************************************************************************//
#*                                                                                                           *//
#*                                   Compute the energy consumption of NoC                                   *//
#*                                                                                                           *//
#*************************************************************************************************************//

# TODO

#*************************************************************************************************************//
#*                                                                                                           *//
#*               Compute energy consumption of instructions (Computation and label accesses)                 *//
#*                                                                                                           *//
#*************************************************************************************************************//

Ins_length = 0 # Size of the instruction in nb of cycles
Inst_complex = 0
inst_count = 0
file_path = path_inputFolder + '/Executed_Computing_Instructions.csv'
f_file = open(file_path,'r')
f_file.readline() #Skip the first line
for line in f_file:
    splitLine = line.split(',')
    #Read the size of an instruction and compute the energy consumed
    Ins_length = int(splitLine[2])
    # Determine Power of the instruction
    # First, determine the complexity of the instruction
    Inst_complex = random.randint(inst_complex_min, inst_complex_max) #Select_complexity_inst(inst_complex_min,inst_complex_max)
    Energy.Tot_energy_exec = Energy.Tot_energy_exec + Tool.get_Energy_Instruction(library,lib,Ins_length,Inst_complex,Platform_C.clock) #Dynamic energy J
    Execution_C.Complexity_mean = Execution_C.Complexity_mean + Inst_complex
    Application_C.Time_ins = Application_C.Time_ins+Ins_length
    inst_count = inst_count + 1

f_file.close

Execution_C.Complexity_mean = Execution_C.Complexity_mean/inst_count
Energy.Power_dynamic_mean = ((library[lib].table_complex_pow)[(int)(Execution_C.Complexity_mean)-1])

# Determine the theoretical number of packets
# Go through the list of all recorded executed Runnables and determine the theoretical total number of packets
file_path = path_inputFolder + '/Executed_Computing_Instructions.csv'
f_file = open(file_path,'r')
f_file.readline() #Skip the first line
for line in f_file:
    splitLine = line.split(',')
    runId = splitLine[0]
    run = Tool.getRun(Application_C.runs,runId)
    Application_C.packets = Application_C.packets + run.Nb_packets;
f_file.close

Application_C.total_flits = Application_C.packets*Platform_C.N_flt_pack #Use label set (local or remote)
Execution_C.remote_flits = Execution_C.N_mesg*Platform_C.N_flt_pack #Additionally use RB and SB

# Compute the energy of instructions (label accesses)
#Dynamic energy of accesses to the label sets (of all cores)
Energy.Dyn_energy_label_set = Labels_C.Dynamic_energy_read_port_label*Application_C.total_flits #nJ

#Dynamic energy of accesses to the RB SB buffers
Energy.Dyn_energy_RB_SB = 2*Buffers_C.Dynamic_energy_read_port_RB_SB*Execution_C.remote_flits #nJ

Energy.Dyn_energy_labels = Energy.Dyn_energy_label_set + Energy.Dyn_energy_RB_SB #nJ

Energy.Stat_energy_label_set = (1+Labels_C.Percentage_refresh_of_leakage/100)*(Execution_C.exec_time*1e-9)*Labels_C.Static_power_bank_label*Labels_C.Nb_banks_labels #J
Energy.Stat_energy_WRB = Buffers_C.Static_power_bank_WRB*Buffers_C.Nb_banks_buffers*(Execution_C.exec_time*1e-9) #J
Energy.Stat_energy_REB = Buffers_C.Static_power_bank_REB*Buffers_C.Nb_banks_buffers*(Execution_C.exec_time*1e-9) #J
Energy.Stat_energy_PRB = Buffers_C.Static_power_bank_PRB*Buffers_C.Nb_banks_buffers*(Execution_C.exec_time*1e-9) #J
Energy.Stat_energy_RB_SB = Buffers_C.Static_power_bank_RB_SB*Buffers_C.Nb_banks_buffers*(Execution_C.exec_time*1e-9) #J

Energy.Stat_energy_labels=Energy.Stat_energy_label_set+Energy.Stat_energy_RB_SB #J
Energy.Tot_energy_label_set = Energy.Stat_energy_label_set+(Energy.Dyn_energy_label_set*1e-9) #J
Energy.Tot_energy_RB_SB = Energy.Stat_energy_RB_SB+Energy.Dyn_energy_RB_SB*1e-9 #J

Energy.Tot_energy_labels = Energy.Tot_energy_label_set+Energy.Tot_energy_RB_SB #J

prop2 = Energy.Stat_energy_labels;
prop1 = Energy.Dyn_energy_labels*1e-9 #J
#prop1 = (Dyn_energy_labels)/(Dyn_energy_labels+Stat_energy_labels);
#prop1 = (Stat_energy_labels)/(Stat_energy_labels+Dyn_energy_labels);
#printf("Dyn_energy_labels : %f\n",prop1);
#printf("Stat_energy_labels : %f\n",prop2);


Energy.Tot_energy_cpu_idle = ((Execution_C.exec_time*(1e-9)*Platform_C.N_processors)*Power_static)*1e+6 #micro J

Energy.Tot_energy_cores = (Energy.Tot_energy_cpu_idle + Energy.Tot_energy_exec*1e+6 + Energy.Tot_energy_labels*1e+6 + Energy.Tot_energy_preemptions*1e-3 + (Energy.Stat_energy_WRB+Energy.Stat_energy_REB+Energy.Stat_energy_PRB)*1e+6) # micro J
Energy.Tot_energy = Energy.Tot_energy_cores+Energy.E_total_NoC # micro J

Energy.E_static_cores = Energy.Tot_energy_cpu_idle + (Energy.Stat_energy_label_set+Energy.Stat_energy_WRB+Energy.Stat_energy_REB+Energy.Stat_energy_PRB+Energy.Stat_energy_RB_SB)*1e+6 #micro J

Energy.E_dynamic_cores = Energy.Tot_energy_preemptions + Energy.Tot_energy_exec*1e+9 + Energy.Dyn_energy_label_set + Energy.Dyn_energy_RB_SB #nJ

Energy.E_static_platform = Energy.E_static_cores+Energy.E_static_NoC
Energy.E_dynamic_platform = (Energy.E_dynamic_cores+Energy.E_dynamic_NoC)*1e-3 #nJ


#*************************************************************************************************************//
#*                                                                                                           *//
#*                                     Generate Output                                                       *//
#*                                                                                                           *//
#*************************************************************************************************************//
print
print"**************************************************************"
print"*                                                            *"
print"* Estimation of the total energy consumption of an           *"
print"* application running on a multi-processor compute platform. *"
print"* The estimated energy is mapping dependednt.                *"
print"* The energy consumption computation, takes into account the *"
print"* energy consumption of cores (instruction execution,        *"
print"* label accesses (local and remote), context switch) and     *"
print"* the energy consumption of NoC.                             *"
print"*                                                            *"
print"**************************************************************"
print
print
print" #Input Configuration Information#"
print
if(Platform_C.cpu_name=="ARM"):
    print"    Type of the core : ARM_A15"

if(Platform_C.cpu_name=="ALP"):
    print"    Type of the core : ALPHA21364"

print"    System  Operating  Frequency :" + str(Platform_C.clock)+ " (GHz)"
print"    Total execution time : "+str(Execution_C.exec_time)+ " (ns)"
print"    Number  of  Rows  in  Platform : "+str(Platform_C.Rows)
print"    Number  of  Columns in  Platform : "+str(Platform_C.Columns)
print"    Number  of  Cores   in  System : "+str(Platform_C.N_processors)
print"    Packets Exchanged through NoC for Remote Label Accesses : "+str(Execution_C.N_mesg)
#Output energies
print
print
print" #Summary of Results#"
print
print"    Energy of NoC (dynamic and static) : "+str(Energy.E_total_NoC)+" (micro J)"
print"    Energy of cores (dynamic and static due to instruction execution, local label accesses and idle state) : "+str(Energy.Tot_energy_cores)+" (micro J)"
print"    Total Energy of the system is : "+str(Energy.Tot_energy)+" (micro J)"
prop1 = ((Energy.Tot_energy_cores/Energy.Tot_energy))
prop2 = ((Energy.E_total_NoC/Energy.Tot_energy))
print"    Proportion of total energy consumption by cores : "+str(prop1*100)
print"    Proportion of total energy consumption by NoC : "+str(prop2*100)
print
print
print
print" #Detailed Results#"
print
#printf("    Number of virtual channels : %d\n", N_channels);
print"    Average packet latency : "+str(Execution_C.Mean_duration_packet)+" (ns)"
print"    Total execution time of computational instructions : "+str(Application_C.Time_ins)+" (cycles)"
print"    Data size for label accesses (local and remote) : "+str(Application_C.packets)+" (number of packets)"
print"    Data size for local label accesses : "+str(Application_C.packets-Execution_C.N_mesg)+" (number of packets)"
print"    Data size for remote label accesses : "+str(Execution_C.N_mesg)+" (number of packets)"
print
print"    Power of an idle cpu : "+str(Power_static)+" (W) static power obtained by McPAT"
print"    Power of a busy cpu : "+str(Energy.Power_dynamic_mean)+" (W) computed as "+str(0.2*Execution_C.Complexity_mean-0.1)+" times the energy of idle core at "+str(config_freq)+" MHz"
print
print"    #Breakdown of Energy Consumption by the platform#"
print"    Static energy platform : "+str(Energy.E_static_platform)+" (micro J)"
print"    Dynamic energy platform : "+str(Energy.E_dynamic_platform)+" (micro J)"
print

print"    #Breakdown of Energy Consumption by Cores#"
print"    Static energy cores : "+str(Energy.E_static_cores)+" (micro J)"
print"    Dynamic energy cores : "+str(Energy.E_dynamic_cores*1e-3)+" (micro J)"
print"    Total energy consumed by labels at core levels : "+str(Energy.Tot_energy_labels*1e+6)+" (micro J)"
print"    Dynamic energy consumed by label accesses at core levels : "+str(Energy.Dyn_energy_labels*1e-3)+" (micro J)"
print"    Dynamic energy consumed by instruction executions : "+str(Energy.Tot_energy_exec*1e+6)+" (micro J)"
print"    Energy of interbuffer runnable transfers : "+str(Energy.Tot_energy_preemptions*1e-3)+" (micro J)"
print"    Total energy consumed by idle cpu : "+str(Energy.Tot_energy_cpu_idle)+" (micro J)"
print
print"    #Breakdown of Energy Consumption by NoC#"
print"    Static energy NoC : "+str(Energy.E_static_NoC)+" (micro J)"
print"    Dynamic energy NoC : "+str(Energy.E_dynamic_NoC*1e+3)+" (nJ)"
print

prop1 = Energy.Tot_energy_cpu_idle/Energy.Tot_energy_cores;
prop2 = ((Energy.Tot_energy_exec*1e+6 + Energy.Tot_energy_labels*1E+6 + Energy.Tot_energy_preemptions*1E-3 + (Energy.Stat_energy_WRB+Energy.Stat_energy_REB+Energy.Stat_energy_PRB)*1e+6)/Energy.Tot_energy_cores);
print"    Proportion of cores energy due to labels (dynamic and static) and instruction execution (simple dynamic and interbuffers runnable transfers) : "+str(prop2*100)
print"    Proportion of cpu energy due to idle state : "+str(prop1*100)
print
print

print"**************************************************************"
print
