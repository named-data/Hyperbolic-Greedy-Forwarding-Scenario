Hyperbolic-Greedy-Forwarding-Scenario
=====================================

Hyperbolic-Greedy-Forwarding-Scenario is a ndnSIM scenario that can be used to run simulations on networks embedded into a hyperbolic space. The scenario contains two tools that can be used as templates for more complex simulations.

This scenario can be built using the usual waf commands:
```bash
./waf configure
./waf
./waf install #optional
```
ndnSIM **must** be installed, also if ndnSIM is installed in path /path_to_ndnsim then, it is recommended to run
```bash
export PKG_CONFIG_PATH=/path_to_ndnsim/lib/pkgconfig
``` 
before the waf configuration step.

The previous steps build two command-line tools: **hg_graph_test** and **hg_greedy_test**.

### hg_graph_test
A template scenario that shows how hyperbolic graphs can be loaded and used in an ndnSIM environment. 

```
NAME: 
	hg_graph_test - create or load a graph into a ndnSIM scenario 
			that simulates a network embedded
			into a hyperbolic space.

SYNOPSIS: 
	hg_graph_test [options] [args] 

DESCRIPTION:
	Build a graph embedded into an hyperbolic space (or load the topology and
	its geometry from a file) according to the specifications described in
	http://dx.doi.org/10.1103/PhysRevE.82.036106.


OPTIONS:
	-c	build the hyperbolic graph according to the following options:
		-n	graph size (number of nodes)
			default value is 1000
		-k	expected average degree
			default value is 10
		-g	expected power-law exponent gamma
			default value is 2 (infinite gamma = 10)
		-t	temperature
			default value is 0 (infinite temperature = 10)
		-z	square root of curvature zeta=sqrt(-K)
			or ratio eta=zeta/T in Soft Configuration Model
			default value is 1
		-s	random seed
			default value is 1

	-f	graph file name, load graph from (.hg) file

	-h	print help menu
	-v	verbose (print information about the graph)
```

### hg_greedy_test
A template scenario that shows how hyperbolic greedy forwarding can be simulated in a ndnSIM environment when the network
is embedded into a hyperbolic space.
```
NAME: 
	hg_greedy_test - test the performances of the greedy forwarding strategy
			 in a ndnSIM scenario that simulates a network embedded
			 into a hyperbolic space.

SYNOPSIS: 
	hg_greedy_test [options] [args] 

DESCRIPTION:
	Build a graph embedded into an hyperbolic space (or load the topology and
	its geometry from a file) according to the specifications described in
	http://dx.doi.org/10.1103/PhysRevE.82.036106 and it computes the greedy
	routing success ratio simulating random nodes sending interest packets
	to other (random) nodes over the network.
	The program outputs the success ratio at the end of the simulation.


OPTIONS:
	-c	build the hyperbolic graph according to the following options:
		-n	graph size (number of nodes)
			default value is 1000
		-k	expected average degree
			default value is 10
		-g	expected power-law exponent gamma
			default value is 2 (infinite gamma = 10)
		-t	temperature
			default value is 0 (infinite temperature = 10)
		-z	square root of curvature zeta=sqrt(-K)
			or ratio eta=zeta/T in Soft Configuration Model
			default value is 1
		-s	random seed
			default value is 1

	-f	graph file name, load graph from (.hg) file

	-a	number of attempts
		default value is 10 x number of nodes
	-h	print help menu
	-v	verbose (print information about the random producers,consumers
		that have been selected for testing)

```

### More information
More information related to the described tools are available in the project wiki:
https://github.com/chiaraorsini/Hyperbolic-Greedy-Forwarding-Scenario/wiki/Hyperbolic-Greedy-Forwarding-Scenario-Guide
