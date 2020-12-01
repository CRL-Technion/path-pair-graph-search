#include <iostream>
#include <memory>

#include "ShortestPathHeuristic.h"
#include "../Utils/Definitions.h"
#include "../Utils/IOUtils.h"
#include "../Utils/Logger.h"
#include "../BiCriteria/BOAStar.h"
#include "../BiCriteria/PPA.h"

const std::string resource_path = "src/Example/Resources/";

// Simple example to demonstarte the usage of the algorithm
void single_run_ny_map(size_t source, size_t target, double eps, LoggerPtr logger) {
    std::cout << "-----Start NY Map Single Example: SRC=" << source << " DEST=" << target << " EPS=" << eps << "-----" << std::endl;

    // Load files
    size_t graph_size;
    std::vector<Edge> edges;
    if (load_gr_files(resource_path+"USA-road-d.NY.gr", resource_path+"USA-road-t.NY.gr", edges, graph_size) == false) {
        std::cout << "Failed to load gr files" << std::endl;
        return;
    }

    std::cout << "Graph Size: " << graph_size << std::endl;

    // Build graphs
    AdjacencyMatrix graph(graph_size, edges);
    AdjacencyMatrix inv_graph(graph_size, edges, true);

    // Compute heuristic
    std::cout << "Start Computing Heuristic" << std::endl;
    ShortestPathHeuristic sp_heuristic(target, graph_size, inv_graph);
    std::cout << "Finish Computing Heuristic\n" << std::endl;

    using std::placeholders::_1;
    Heuristic heuristic = std::bind( &ShortestPathHeuristic::operator(), sp_heuristic, _1);

    // Compute BOAStar
    std::cout << "Start Computing BOAStar" << std::endl;
    SolutionSet boa_solutions;
    BOAStar boa_star(graph, {eps,eps}, logger);
    boa_star(source, target, heuristic, boa_solutions);
    std::cout << "Finish Computing BOAStar" << std::endl;

    std::cout << "BOAStar Solutions:" << std::endl;
    size_t solutions_count = 0;
    for (auto solution = boa_solutions.begin(); solution != boa_solutions.end(); solution++) {
        std::cout << ++solutions_count << ". " << *(*solution) << std::endl;
    }
    std::cout << std::endl;

    // Compute PPA
    std::cout << "Start Computing PPA" << std::endl;
    SolutionSet ppa_solutions;
    PPA ppa(graph, {eps,eps}, logger);
    ppa(source, target, heuristic, ppa_solutions);
    std::cout << "Finish Computing PPA" << std::endl;

    solutions_count = 0;
    for (auto solution = ppa_solutions.begin(); solution != ppa_solutions.end(); solution++) {
        std::cout << ++solutions_count << ". " << *(*solution) << std::endl;
    }
    std::cout << std::endl;

    std::cout << "-----End NY Map Single Example-----" << std::endl;
}


void run_queries(std::string map, double eps, LoggerPtr logger) {
    std::cout << "-----Start " << map << " Map Queries Example: EPS=" << eps << "-----" << std::endl;

    // Load files
    size_t graph_size;
    std::vector<Edge> edges;
    if (load_gr_files(resource_path+"USA-road-d."+map+".gr", resource_path+"USA-road-t."+map+".gr", edges, graph_size) == false) {
        std::cout << "Failed to load gr files" << std::endl;
        return;
    }

    std::vector<std::pair<size_t, size_t>> queries;
    if (load_queries(resource_path+"USA-road-"+map+"-queries", queries) == false) {
        std::cout << "Failed to load queries file" << std::endl;
        return;
    }

    // Build graphs
    AdjacencyMatrix graph(graph_size, edges);
    AdjacencyMatrix inv_graph(graph_size, edges, true);

    size_t query_count = 0;
    for (auto iter = queries.begin(); iter != queries.end(); ++iter) {
        std::cout << "Started Query: " << ++query_count << "/" << queries.size() << std::endl;
        size_t source = iter->first;
        size_t target = iter->second;

        ShortestPathHeuristic sp_heuristic(target, graph_size, inv_graph);

        using std::placeholders::_1;
        Heuristic heuristic = std::bind( &ShortestPathHeuristic::operator(), sp_heuristic, _1);

        SolutionSet boa_solutions;
        BOAStar boa_star(graph, {eps,eps}, logger);
        boa_star(source, target, heuristic, boa_solutions);

        SolutionSet ppa_solutions;
        PPA ppa(graph, {eps,eps}, logger);
        ppa(source, target, heuristic, ppa_solutions);
    }

    std::cout << "-----End " << map << " Map Queries Example-----" << std::endl;
}


// Run all queries on all availible maps. The logs outputed from this function are
// used for running the tests
void run_all_queries(void) {
    LoggerPtr logger_bay = new Logger("queries_BAY_log.json");
    run_queries("BAY", 0.1, logger_bay);
    run_queries("BAY", 0.05, logger_bay);
    run_queries("BAY", 0.025, logger_bay);
    run_queries("BAY", 0.01, logger_bay);
    run_queries("BAY", 0, logger_bay);
    delete logger_bay;

    LoggerPtr logger_col = new Logger("queries_COL_log.json");
    run_queries("COL", 0.1, logger_col);
    run_queries("COL", 0.05, logger_col);
    run_queries("COL", 0.025, logger_col);
    run_queries("COL", 0.01, logger_col);
    run_queries("COL", 0, logger_col);
    delete logger_col;

    LoggerPtr logger_ne = new Logger("queries_NE_log.json");
    run_queries("NE", 0.1, logger_ne);
    run_queries("NE", 0.05, logger_ne);
    run_queries("NE", 0.025, logger_ne);
    run_queries("NE", 0.01, logger_ne);
    run_queries("NE", 0, logger_ne);
    delete logger_ne;

    LoggerPtr logger_ny = new Logger("queries_NY_log.json");
    run_queries("NY", 0.1, logger_ny);
    run_queries("NY", 0.05, logger_ny);
    run_queries("NY", 0.025, logger_ny);
    run_queries("NY", 0.01, logger_ny);
    run_queries("NY", 0, logger_ny);
    delete logger_ny;
}


int main(void) {
    LoggerPtr logger = new Logger("example_log.json");
    // Easy - Benchmark C_BOA code gets around 20ms
    size_t easy_source = 9899;
    size_t easy_target = 7857;
    single_run_ny_map(easy_source, easy_target, 0, logger);

    // Hard - Benchmark C_BOA code gets around 2k ms
    size_t hard_source = 180834;
    size_t hard_target = 83150;
    single_run_ny_map(hard_source, hard_target, 0, logger);
    delete logger;

    // try {
    //     run_all_queries();
    // } catch (const std::exception &e) {
    //     std::cout << "Exception: " << e.what() << std::endl;
    // }
    return 0;
}

