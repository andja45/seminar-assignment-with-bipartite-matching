#ifndef SEMINARALLOCATIONENGINE_ROUNDHISTORY_H
#define SEMINARALLOCATIONENGINE_ROUNDHISTORY_H

#include <vector>
#include <utility>
#include <unordered_map>
#include <unordered_set>
#include "ExamRound.h"
#include "matching/MatchingResult.h"

struct RoundHistory {
    std::vector<ExamRound> completedPeriods;
    std::unordered_map<int, std::vector<std::pair<ExamRound,int>>> assignments; // studentId -> [(period, topicId)]

    std::unordered_set<int> forbiddenTopics(int studentId) const;
    std::unordered_map<int, std::unordered_set<int>> forbiddenMap() const; // (student, forbidden topics)
    int unmatchedRounds(int studentId) const;
    void commit(ExamRound, const MatchingResult&);
};

#endif
