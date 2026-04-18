#ifndef SEMINARALLOCATIONENGINE_CRITICALITYANALYZER_H
#define SEMINARALLOCATIONENGINE_CRITICALITYANALYZER_H

#include <unordered_map>
#include "graph/AllocationGraph.h"
#include "matching/MatchingResult.h"
#include "KonigCover.h"

enum class CoverCriticality {
    Sole,    // there exists at least one unmatched student whose edge only this node covers
    Shared,  // every edge through this node is also covered by another cover node
    Marginal // no unmatched student preference edge passes through this node
};

struct CriticalityResult {
    std::unordered_map<int, CoverCriticality> level; // coverNodeId -> criticality
};

CriticalityResult analyzeCriticality(const AllocationGraph& graph, const MatchingResult& matching, const CoverResult& cover);

#endif
