#ifndef SEMINARALLOCATIONENGINE_MATCHINGRESULT_H
#define SEMINARALLOCATIONENGINE_MATCHINGRESULT_H

#include <unordered_map>
#include <vector>

struct MatchingResult {
    std::unordered_map<int,int> studentMatch;
    std::unordered_map<int,int> topicMatch;
    int matchingSize = 0;
    int augmentations = 0;
    int phases = 0;
    int edgeTraversals = 0;
    std::vector<std::vector<std::vector<int>>> augPathNodes; // for each phase, all augmenting paths found
};

#endif
