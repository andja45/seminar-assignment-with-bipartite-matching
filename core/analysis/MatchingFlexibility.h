#ifndef SEMINARALLOCATIONENGINE_MATCHINGFLEXIBILITY_H
#define SEMINARALLOCATIONENGINE_MATCHINGFLEXIBILITY_H

#include <unordered_map>
#include <vector>
#include "graph/AllocationGraph.h"
#include "matching/MatchingResult.h"

enum class FlexibilityLevel {
    Forced,   // every maximum matching assigns this student to this exact topic — no alternative exists
    Flexible  // there exists another maximum matching where this student gets a different topic
};

struct MatchingFlexibilityResult {
    std::unordered_map<int, FlexibilityLevel> studentLevel;
    std::unordered_map<int, std::vector<int>> alternativeTopics; // (Flexible studentId, topics they could swap to)

    // forced / total matched: 1.0 = fully rigid matching, 0.0 = every assignment is interchangeable
    float flexibilityIndex;
};

MatchingFlexibilityResult computeFlexibility(AllocationGraph& graph, const MatchingResult& matching);

#endif
