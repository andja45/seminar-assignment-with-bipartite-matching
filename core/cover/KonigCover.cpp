#include "KonigCover.h"
#include <queue>

CoverResult computeCover(const AllocationGraph& graph, const MatchingResult& matching) {
    CoverResult result;

    std::unordered_set<int> zStudents;
    std::unordered_set<int> zTopics;

    std::queue<int> q;

    // we start with unmatched students
    for (int s : graph.getByType(NodeType::Student)) {
        if (matching.studentMatch.at(s) == -1) {
            zStudents.insert(s);
            q.push(s);
        }
    }

    while (!q.empty()) {
        int s = q.front(); q.pop();

        std::vector<int> step;

        for (int t : graph.prefsOf(s)) {
            if (zTopics.count(t)) continue; // already reached this topic

            // follow free preference edge s→t (any topic s wants is reachable)
            zTopics.insert(t);
            step.push_back(t);

            // following t->s' (if t is matched to s'), student who is holding t
            int holder = matching.topicMatch.at(t);
            if (holder != -1 && !zStudents.count(holder)) {
                zStudents.insert(holder);
                step.push_back(holder);
                q.push(holder); // explore their preferences next
            }
        }

        if (!step.empty())
            result.buildSteps.push_back(step);
    }

    // cover = students not in Z (they block the topic they are matched to)
    for (int s : graph.getByType(NodeType::Student)) {
        if (matching.studentMatch.at(s) != -1 && !zStudents.count(s))
            result.coverStudents.insert(s);
    }

    // cover = topics in Z (that topic is claimed and blocked for any other student)
    result.coverTopics = zTopics;

    // Konig's theorem: |cover| == matching size
    return result;
}
