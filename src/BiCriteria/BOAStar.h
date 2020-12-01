#ifndef BI_CRITERIA_BOA_STAR_H
#define BI_CRITERIA_BOA_STAR_H

#include <vector>
#include "../Utils/Definitions.h"
#include "../Utils/Logger.h"

class BOAStar {
private:
    const AdjacencyMatrix   &adj_matrix;
    Pair<double>            eps;
    const LoggerPtr         logger;

    void start_logging(size_t source, size_t target);
    void end_logging(SolutionSet &solutions);

public:
    BOAStar(const AdjacencyMatrix &adj_matrix, Pair<double> eps, const LoggerPtr logger=nullptr);
    void operator()(size_t source, size_t target, Heuristic &heuristic, SolutionSet &solutions);
};

#endif //BI_CRITERIA_BOA_STAR_H