#ifndef SEMINARALLOCATIONENGINE_KONIGCOVER_H
#define SEMINARALLOCATIONENGINE_KONIGCOVER_H

#include <unordered_set>
#include <vector>
#include "graph/AllocationGraph.h"
#include "matching/MatchingResult.h"

struct CoverResult {
    std::unordered_set<int> coverStudents;
    std::unordered_set<int> coverTopics;

    std::vector<std::vector<int>> buildSteps;
};

CoverResult computeCover(const AllocationGraph& graph, const MatchingResult& matching);

#endif
