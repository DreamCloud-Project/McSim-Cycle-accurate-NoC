# McSim - NoC-based platform cycle-accurate simulator

This repository contains a simulator able to simulate embedded applications described using
the [AMALTHEA](http://www.amalthea-project.org/) application model on top of NoC-based architectures in a cycle accurate way.

## Using the simulator

To ease the usage of the simulator, two python scripts are provided:  

- compile.py for compilation  
- simulate.py for launching the simulation  

The requirements for using these scripts are the following ones:  

- have CMake installed on your system [(https://cmake.org)](https://cmake.org/)
- define the SYSTEMC_HOME variable pointing to a SystemC 2.3.1 root folder
- have the xerces-c-dev library installed in standard includes and libs folders (using apt-get for example)
  or have xerces-c-dev library in a custom folder and define XERCES_HOME

### Compiling the simulator

Compilation is done through the compile.py script which documentation is the following:  

```
TODO
```

### Running the simulator

To run a particular simulation, just run the simulate.py script. By
default this script runs one iteration of the Demo Car application on
a 4x4 NoC using ZigZag mapping, First Come First Serve (fcfs)
scheduling and without repeating periodic runnables.  You can play
with all these parameters which documentation is the following:

```
TODO
```

## Licence

This software is made available under the  GNU Lesser General Public License v3.0

Report bugs at: mcsim-support@lirmm.fr  

(C)2015 CNRS LIRMM / Université de Montpellier
