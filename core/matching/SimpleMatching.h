#ifndef SEMINARALLOCATIONENGINE_SIMPLEMATCHING_H
#define SEMINARALLOCATIONENGINE_SIMPLEMATCHING_H

#include "MatchingResult.h"
#include "graph/AllocationGraph.h"

// finds one augmenting path from start via BFS, returns node sequence or empty if none exists
std::vector<int> findAugmentingPath(int start, const AllocationGraph& graph,
    const std::unordered_map<int,int>& studentMatch,
    const std::unordered_map<int,int>& topicMatch,
    int& edgeTraversals);

MatchingResult runSimpleMatching(const AllocationGraph& graph);

#endif
