# OpenCL-Communication-Simulation
## Overview
This project focuses on simulating parallel communication using OpenCL for two scenarios: Scatterv and Gatherv. The assignment is part of the Parallel and Distributed Computing course, and it involves implementing separate OpenCL programs for each scenario, mimicking the functionality of MPI processes. Proper synchronizations are considered in the communication through a common memory.

### Scatterv Implementation
### Concept-1
* Scatterv Simulation:

Ten processes simulate communication equivalent to MPI Scatterv using OpenCL.
One process is randomly selected as the coordinating process, whose buffer will be scattered.
Data buffer to be scattered is dynamically created with elements ranging from 1000 to 10000.
Each process initializes its data buffer, and the coordinating process generates random sendcounts and displacements for 10 processes.
After scattering data, each process calculates the sum of received data values.
Local sums are communicated to the main program (host), which computes the global sum and displays the result.
### Gatherv Implementation
### Concept-2
* Gatherv Simulation:

Ten processes simulate communication equivalent to MPI Gatherv using OpenCL.
One process is randomly selected as the coordinating process, whose buffer will be prepared by gathering data sent by others.
Each process dynamically creates data of size ranging from 20 to 200 elements and initializes it.
Sendcounts and displacements data structures are dynamically/randomly prepared for 10 processes and used by the coordinating process.
After gathering data, the coordinating process calculates a dot product using a randomly created and prepared sparse data array.
The coordinating process conveys the dot product data to the main program (host).
The host program sums all values and displays the result.
### Getting Started
* Clone the Repository:

git clone https://github.com/your-username/opencl-communication-simulation.git
* Build and Run:

Navigate to the project directory and compile the OpenCL code.

make
Execute the compiled program.

./scatterv
./gatherv
Review Results:

Examine the output to see the simulated communication results for Scatterv and Gatherv scenarios.
### Dependencies
OpenCL library for parallel computing
### Contributing
Feel free to contribute by opening issues or submitting pull requests. Your feedback and improvements are highly appreciated.
