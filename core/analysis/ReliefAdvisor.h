#ifndef SEMINARALLOCATIONENGINE_RELIEFADVISOR_H
#define SEMINARALLOCATIONENGINE_RELIEFADVISOR_H

#include <unordered_map>
#include <vector>
#include "graph/AllocationGraph.h"
#include "matching/MatchingResult.h"

struct ReliefResult {
    std::unordered_map<int, std::vector<int>> perStudent;   // (unmatched studentId, list of topicIds that would unlock their assignment)
    std::unordered_map<int, int> topicGain; // (topicId, num of student that could get assigned by it)
};

ReliefResult computeRelief(AllocationGraph& graph, const MatchingResult& matching);

#endif
