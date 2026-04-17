#include "CriticalityAnalyzer.h"

CriticalityResult analyzeCriticality(const AllocationGraph& graph, const MatchingResult& matching, const CoverResult& cover){
    CriticalityResult result;

    // every node starts as Marginal
    for (int s : cover.coverStudents) result.level[s] = CoverCriticality::Marginal;
    for (int t : cover.coverTopics)   result.level[t] = CoverCriticality::Marginal;

    // walk every unmatched student's preference edges
    for (int s : graph.getByType(NodeType::Student)) {
        if (matching.studentMatch.at(s) != -1) continue; // skip matched students

        for (int t : graph.prefsOf(s)) {
            // check which cover nodes sit on this edge s→t
            bool studentInCover = cover.coverStudents.count(s);
            bool topicInCover = cover.coverTopics.count(t);

            if (!studentInCover && !topicInCover) continue; // shouldn't happen — means matching is not maximum

            if (studentInCover && topicInCover) {
                // both endpoints are in the cover, neither is sole responsible
                // only Marginal->Shared, Sole->Shared cannot happen (we need only one such edge to keep a node Sole, indicating potential for another matching!)
                if (result.level[s] == CoverCriticality::Marginal) result.level[s] = CoverCriticality::Shared;
                if (result.level[t] == CoverCriticality::Marginal) result.level[t] = CoverCriticality::Shared;
            } else if (studentInCover) {
                // only the student end is in cover — this student is solely responsible
                result.level[s] = CoverCriticality::Sole;
            } else {
                // only the topic end is in cover — this topic is solely responsible
                result.level[t] = CoverCriticality::Sole;
            }
        }
    }

    return result;
}
