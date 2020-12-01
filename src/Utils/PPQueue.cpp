#include <algorithm>

#include "PPQueue.h"

PPQueue::PPQueue(size_t graph_size)
    : open_map(graph_size, std::vector<PathPairPtr>()) {

    std::make_heap(this->heap.begin(), this->heap.end(), this->more_than);
}

bool PPQueue::empty() {
    return this->heap.empty();
}

PathPairPtr PPQueue::top() {
    return this->heap.back();
}

PathPairPtr PPQueue::pop() {
    // Pop from min heap
    std::pop_heap(this->heap.begin(), this->heap.end(), this->more_than);
    PathPairPtr pp = this->heap.back();
    this->heap.pop_back();

    // Remove from open map
    std::vector<PathPairPtr> &relevant_pps = this->open_map[pp->id];
    for (auto iter = relevant_pps.begin(); iter != relevant_pps.end(); ++iter) {
        if (pp == *iter) {
            relevant_pps.erase(iter);
            break;
        }
    }

    return pp;
}

void PPQueue::insert(PathPairPtr &pp) {
    // Insert to min heap
    this->heap.push_back(pp);
    std::push_heap(this->heap.begin(), this->heap.end(), this->more_than);

    // Insert to open map
    this->open_map[pp->id].push_back(pp);
}

std::vector<PathPairPtr> &PPQueue::get_open_pps(size_t id) {
	return this->open_map[id];
}
