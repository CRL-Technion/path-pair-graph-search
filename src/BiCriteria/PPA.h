#ifndef BI_CRITERIA_PPA_H
#define BI_CRITERIA_PPA_H

#include "../Utils/Definitions.h"
#include "../Utils/Logger.h"
#include "../Utils/PPQueue.h"


class PPA {
private:
    const AdjacencyMatrix   &adj_matrix;
    Pair<double>            eps;
    const LoggerPtr         logger;

    void start_logging(size_t source, size_t target);
    void end_logging(SolutionSet &solutions);

    void insert(PathPairPtr &pp, PPQueue &queue);
    void merge_to_solutions(const PathPairPtr &pp, PPSolutionSet &solutions);

public:
    PPA(const AdjacencyMatrix &adj_matrix, Pair<double> eps, const LoggerPtr logger=nullptr);
    void operator()(size_t source, size_t target, Heuristic &heuristic, SolutionSet &solutions);
};

#endif //BI_CRITERIA_PPA_H