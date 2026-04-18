#ifndef SEMINARALLOCATIONENGINE_ALLOCATIONEXAMPLES_H
#define SEMINARALLOCATIONENGINE_ALLOCATIONEXAMPLES_H

#include <string>
#include "graph/AllocationGraph.h"
#include "rounds/RoundHistory.h"

struct Example {
    std::string name;
    AllocationGraph graph;
    RoundHistory history; // empty for single-round examples
};

// 8 students, 5 topics — many students want the same topics, matching must leave some unmatched.
Example buildBottleneckRound();

// 7 students, 5 topics — structured so HK finishes in 2 phases, Simple needs 5 augmentations.
Example buildLayeredPhases();

// 4 students, 4 topics, 2 exam rounds — round 1 matches one pair, round 2 they rotate.
Example buildRoundFairness();

#endif