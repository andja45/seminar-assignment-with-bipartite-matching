#include "ReliefAdvisor.h"
#include "matching/SimpleMatching.h"
#include <algorithm>
#include <climits>

ReliefResult computeRelief(AllocationGraph& graph, const MatchingResult& matching) {
    ReliefResult result;

    std::vector<int> allTopics = graph.getByType(NodeType::Topic);

    for (int s : graph.getByType(NodeType::Student)) {
        if (matching.studentMatch.at(s) != -1) continue;

        for (int t : allTopics) {
            // skip topics the student already listed — not a new suggestion
            const auto& prefs = graph.prefsOf(s);
            if (std::find(prefs.begin(), prefs.end(), t) != prefs.end()) continue;

            // temporarily add fake preference s→t
            graph.addPreference(s, t, INT_MAX); // INT_MAX rank — won't collide with real ranks
            int dummy = 0;
            bool pathFound = !findAugmentingPath(s, graph, matching.studentMatch, matching.topicMatch, dummy).empty();
            graph.removePreference(s, t);

            if (pathFound) {
                result.perStudent[s].push_back(t);
                result.topicGain[t]++;
            }
        }
    }

    return result;
}
