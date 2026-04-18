#include "MatchingFlexibility.h"
#include "matching/SimpleMatching.h"

MatchingFlexibilityResult computeFlexibility(AllocationGraph& graph, const MatchingResult& matching) {
    MatchingFlexibilityResult result;

    auto studentMatchCopy= matching.studentMatch;
    auto topicMatchCopy= matching.topicMatch;

    int forcedCount = 0;

    for (int s : graph.getByType(NodeType::Student)) {
        auto it = matching.studentMatch.find(s);
        if (it == matching.studentMatch.end() || it->second == -1) continue; // we only look at matched students

        int t = it->second;

        int originalRank = 0;
        for (const auto& pref : graph.preferences())
            if (pref.student == s && pref.topic == t) { originalRank = pref.rank; break; }

        studentMatchCopy[s] = -1;
        // keep topicMatchCopy[t] = s — if freed, BFS would grab t back as a free endpoint

        graph.removePreference(s, t); // block s->t so BFS cannot immediately take the same topic

        // can s reach a free topic via swap chains?
        int dummy = 0;
        auto path = findAugmentingPath(s, graph, studentMatchCopy, topicMatchCopy, dummy);

        if (path.empty()) {
            result.studentLevel[s] = FlexibilityLevel::Forced;
            forcedCount++;
        } else {
            result.studentLevel[s] = FlexibilityLevel::Flexible;
            // path layout: [s, t0, s1, t1, ..., sN, tFree] — last node is the alternative topic
            result.alternativeTopics[s].push_back(path.back());
        }

        studentMatchCopy[s] = t;
        graph.addPreference(s, t, originalRank);
    }

    int totalMatched = (int)result.studentLevel.size();
    result.flexibilityIndex = (totalMatched > 0)
        ? (float)forcedCount / (float)totalMatched
        : 0.0f;

    return result;
}
