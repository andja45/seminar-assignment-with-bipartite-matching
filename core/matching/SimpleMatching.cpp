#include "SimpleMatching.h"
#include <queue>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>

// BFS algorithm on the bipartite preference graph
std::vector<int> findAugmentingPath(int start, const AllocationGraph& graph,
    const std::unordered_map<int,int>& studentMatch, const std::unordered_map<int,int>& topicMatch, int& edgeTraversals) {

    std::unordered_map<int,int> parent; //(topicId, studentId)
    std::unordered_set<int> visited;
    std::queue<int> q;

    q.push(start);
    visited.insert(start);

    while (!q.empty()) {
        int s = q.front(); q.pop();

        for (int t : graph.prefsOf(s)) {
            edgeTraversals++;
            if (parent.count(t)) continue; // already visited this topic
            parent[t] = s;

            if (topicMatch.at(t) == -1) {
                // free topic found — reconstruct path backwards
                std::vector<int> path;
                int topic = t;
                while (topic != -1) {
                    int student = parent[topic];
                    path.push_back(student);
                    path.push_back(topic);
                    topic = studentMatch.at(student);
                }
                std::reverse(path.begin(), path.end());
                return path;
            }

            // topic is taken, BFS from that student
            int holder = topicMatch.at(t);
            if (!visited.count(holder)) {
                visited.insert(holder);
                q.push(holder);
            }
        }
    }

    return {};
}

static void augment(
    const std::vector<int>& path,
    std::unordered_map<int,int>& studentMatch,
    std::unordered_map<int,int>& topicMatch)
{
    for (int i = 0; i < (int)path.size(); i += 2) {
        int s = path[i], t = path[i + 1];
        studentMatch[s] = t;
        topicMatch[t]   = s;
    }
}

MatchingResult runSimpleMatching(const AllocationGraph& graph) {
    MatchingResult result;

    for (int s : graph.getByType(NodeType::Student)) result.studentMatch[s] = -1;
    for (int t : graph.getByType(NodeType::Topic)) result.topicMatch[t] = -1;

    for (int s : graph.getByType(NodeType::Student)) {
        if (result.studentMatch[s] != -1) continue; // already matched

        auto path = findAugmentingPath(
            s, graph, result.studentMatch, result.topicMatch, result.edgeTraversals);

        if (path.empty()) continue; // no augmenting path, stays unmatched

        augment(path, result.studentMatch, result.topicMatch);
        result.matchingSize++;
        result.augmentations++;
        result.phases++;
        result.augPathNodes.push_back({path});
    }

    return result;
}
