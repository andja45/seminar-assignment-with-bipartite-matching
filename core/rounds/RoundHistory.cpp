#include "RoundHistory.h"

// topics that student already took before
std::unordered_set<int> RoundHistory::forbiddenTopics(int studentId) const {
    std::unordered_set<int> result;
    auto it = assignments.find(studentId);
    if (it == assignments.end()) return result;
    for (const auto& [period, topicId] : it->second) result.insert(topicId);
    return result;
}

std::unordered_map<int, std::unordered_set<int>> RoundHistory::forbiddenMap() const {
    std::unordered_map<int, std::unordered_set<int>> result;
    for (const auto& [studentId, assignments] : assignments)
        for (const auto& [period, topicId] : assignments)
            result[studentId].insert(topicId);
    return result;
}

int RoundHistory::unmatchedRounds(int studentId) const {
    int matched = 0;
    auto it = assignments.find(studentId);
    if (it != assignments.end()) matched = static_cast<int>(it->second.size()); // each time student got matched with a topic
    return static_cast<int>(completedPeriods.size()) - matched;
}

void RoundHistory::commit(ExamRound period, const MatchingResult& result) {
    completedPeriods.push_back(period);
    for (const auto& [studentId, topicId] : result.studentMatch)
        if (topicId != -1) assignments[studentId].push_back({period, topicId});
}
