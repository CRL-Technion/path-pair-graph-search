# BiCriteria Search
## Table of Contents
* Section 1: Environment installtion and dependencies
* Section 2: How to run examples and tests
* Section 3: Usage of the code
* Section 4: Algorithmic overview


## Environment installtion and dependencies
* The code was ran and tested under:
	* Macos Catalina with clang 12 compiler
	* Linux Ubuntu with gcc 9.3 compiler
	* Windows 10 with mingw gcc 9.3 compiler
* For compilation of the source code there are no additional dependencies
* For testing and log analization tools we have the following dependencies:
	* Python 3.8.5
	* pandas (pip3 install pandas)


## How to run examples and tests
* Examples of the code usage can be found under src/Example/run_example.cpp
* To run examples download the following maps from [DIMACS](http://users.diag.uniroma1.it/challenge9/download.shtml) challenge site and place them under src/Example/Resources directory:
	* NY distance + time graph (gr format)
	* BAY distance + time graph (gr format)
	* COL distance + time graph (gr format)
	* NE distance + time graph (gr format)
* A simple example can be found under single_run_ny_map function
* A very extensive example (several hours of runtime) is found under run_all_queries function.
* To run the examples:
	* From root directory run "make rebuild"
	* Run "./build/example"
* To run the tests:
	* Run "run_all_queries" function as described in run examples.
	* Run ./test/test.py "root directory of generated logs from the previous step" "filename for the test results"
	* The tests compare the results against precalculated bench marks, to ensure that every solution in the full pareto set is eps dominated by a solution in the apporximated pareto set.
* To run log analization:
	* Run ./src/Utils/log_analyser.py "path to log file" "output format"
	* Output format can be "csv" or "json"


## Usage of the code
```
#include "src/BiCriteria/PPA.h"

SolutionSet get_apx_pareto_set(std::vector<Edge> &edges) {
	// Load the search graph into adjacency matrix. Can be done using std::vector
	// of edges at construction or adding the nodes one by one using .add method.
	AdjacencyMatrix graph(graph_size, edges);

	// Assign heuristic function/functor that takes size_t (node it) as parameter
	// and return Pair<size_t> (heuristic cost for cost1 and cost2)
	YourHeuristic your_heuristic(args);
	Heuristic heuristic = std::bind(&YourHeuristic::operator(), your_heuristic, _1);

	// Run the desired search by passing calling the PPA functor. The solutions will
	// be returned into the passed SolutionSet. You can pass as argument a Logger
	// instance if you desire logging.
    SolutionSet solutions;
    PPA ppa(graph, {eps,eps}, logger);
    ppa(source, target, heuristic, solutions);

    return solutions
}
```
* Compile your code together with PPA source files.


## Algorithmic overview
* TODO attach paper after it is published

