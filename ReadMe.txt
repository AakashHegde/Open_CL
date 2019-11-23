The following programs utilise multiple processing units to acheive a speedup.

There are two types of implementations in this project : 
	1) GPGPU programming using OpenCL
	2) Multi-thread programming using OpenMP

OpenCL:
Introductory programs 	- print "hello world"
			- add a vector of large size
Uses an older version of OpenCL API for compatibility reasons.
No speedup is obtained because of how basic the programs are.

OpenMP:
Program to perform Bitonic Sort - a parallel sort algorithm.
Good speedup observed when workload is appropriate.
Compares the speedup for different array sizes, and displays all timings in a tabulated manner.
