#ifndef UTILS_PP_QUEUE_H
#define UTILS_PP_QUEUE_H

#include <set>
#include <vector>
#include <list>
#include "../Utils/Definitions.h"

class PPQueue
{
private:
    std::vector<PathPairPtr>                heap;
    PathPair::more_than_full_cost           more_than;

    std::vector<std::vector<PathPairPtr>>   open_map;

public:
    PPQueue(size_t graph_size);
    bool empty();
    PathPairPtr top();
    PathPairPtr pop();
    void insert(PathPairPtr &pp);
    std::vector<PathPairPtr> &get_open_pps(size_t id);

};

#endif //UTILS_PP_QUEUE_H

