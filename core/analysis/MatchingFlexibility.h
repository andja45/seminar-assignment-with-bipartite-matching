#ifndef SEMINARALLOCATIONENGINE_ASSIGNMENTNECESSITY_H
#define SEMINARALLOCATIONENGINE_ASSIGNMENTNECESSITY_H

#include <unordered_map>
#include <vector>
#include "graph/AllocationGraph.h"
#include "matching/MatchingResult.h"

enum class NecessityLevel {
    Forced,   // every maximum matching assigns this student to this exact topic — no alternative exists
    Flexible  // there exists another maximum matching where this student gets a different topic
};

struct AssignmentNecessityResult {
    std::unordered_map<int, NecessityLevel> studentLevel;
    std::unordered_map<int, std::vector<int>> alternativeTopics; // (Flexible studentId, topics they could swap to)

    // forced / total matched: 1.0 = fully rigid matching, 0.0 = every assignment is interchangeable
    float necessityIndex;
};

AssignmentNecessityResult computeNecessity(AllocationGraph& graph, const MatchingResult& matching);

#endif
