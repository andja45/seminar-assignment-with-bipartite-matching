#ifndef SEMINARALLOCATIONENGINE_DEMANDCOVERANALYSIS_H
#define SEMINARALLOCATIONENGINE_DEMANDCOVERANALYSIS_H

#include <unordered_map>
#include "../graph/AllocationGraph.h"
#include "../cover/KonigCover.h"

enum class DCALabel {
    Confirmed,  // high demand and in cover — König walk reached it; expanding it would benefit an unmatched student
    Resolved,   // high demand and not in cover — König walk did not reach it; demand was fully absorbed by the matching
    Structural, // low demand and in cover — König walk reached it via swap chain despite low demand
    Clear       // low demand and not in cover — unreachable from any unmatched student; irrelevant to matching outcome
};

struct TopicDCA {
    int demand;    // number of students who want this topic
    bool inCover;  // whether this topic ended up in the König cover
    DCALabel label;
};

struct DCAResult {
    std::unordered_map<int, TopicDCA> topics;

    // fraction of high-demand topics the matching successfully resolved
    // 1.0 = matching absorbed all demand pressure; 0.0 = failed on every contested topic
    // -1.0 = undefined (no high-demand topics)
    float absorptionRate;
};

DCAResult computeDCA(const AllocationGraph& graph, const CoverResult& cover);

#endif
