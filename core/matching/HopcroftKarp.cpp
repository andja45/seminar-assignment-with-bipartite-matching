#include "HopcroftKarp.h"
#include <queue>
#include <unordered_map>
#include <limits>

static constexpr int INF = std::numeric_limits<int>::max();

// BFS phase: builds layered graph from all unmatched students simultaneously
// dist[s] = layer of student s (distance from nearest unmatched source)
static bool bfsHK(const AllocationGraph& graph, const std::unordered_map<int,int>& studentMatch,
    const std::unordered_map<int,int>& topicMatch, std::unordered_map<int,int>& dist, int& edgeTraversals) {
    
    std::queue<int> q;
    bool freeTopicFound = false;

    for (int s : graph.getByType(NodeType::Student)) {
        if (studentMatch.at(s) == -1) {
            dist[s] = 0; // unmatched student - source of BFS
            q.push(s);
        } else {
            dist[s] = INF; // matched students - not yet reachable
        }
    }

    while (!q.empty()) {
        int s = q.front(); q.pop();

        for (int t : graph.prefsOf(s)) {
            edgeTraversals++;
            int s2 = topicMatch.at(t);

            if (s2 == -1) {
                freeTopicFound = true; // free topic reachable from s
                continue;
            }

            if (dist[s2] == INF) { // not yet visited
                dist[s2] = dist[s] + 1;
                q.push(s2);
            }
        }
    }

    return freeTopicFound;
}

// DFS phase: finds one augmenting path from s through the layered graph
// only follows edges to the next layer; prunes s if no path found
static bool dfsHK(int s, const AllocationGraph& graph, std::unordered_map<int,int>& studentMatch,
    std::unordered_map<int,int>& topicMatch, std::unordered_map<int,int>& dist, std::vector<int>& path) {

    for (int t : graph.prefsOf(s)) {
        int s2 = topicMatch.at(t);

        if (s2 == -1) {
            // free topic found — augment this edge
            path.push_back(s);
            path.push_back(t);
            topicMatch[t] = s;
            studentMatch[s] = t;
            return true;
        }

        if (dist[s2] == dist[s] + 1) { // edge to the next layer
            path.push_back(s);
            path.push_back(t);
            if (dfsHK(s2, graph, studentMatch, topicMatch, dist, path)) {
                // path from s2 to a free topic found — augment this edge
                topicMatch[t] = s;
                studentMatch[s] = t;
                return true;
            }
            // no path from s2 to a free topic — backtrack and try another edge
            path.pop_back();
            path.pop_back();
        }
    }

    dist[s] = INF; // no path through s this phase — prune
    return false;
}

// each BFS phase finds minimum augmenting path length, DFS extracts all disjoint paths of that length
MatchingResult runHopcroftKarp(const AllocationGraph& graph) {
    MatchingResult result;

    for (int s : graph.getByType(NodeType::Student)) result.studentMatch[s] = -1;
    for (int t : graph.getByType(NodeType::Topic))   result.topicMatch[t]   = -1;

    std::unordered_map<int,int> dist;

    while (bfsHK(graph, result.studentMatch, result.topicMatch, dist, result.edgeTraversals)) {
        // phase is skipped if BFS found no free topics at that length
        result.phases++;
        std::vector<std::vector<int>> phasePaths;

        for (int s : graph.getByType(NodeType::Student)) {
            if (result.studentMatch[s] != -1) continue; // already matched

            std::vector<int> path;
            if (dfsHK(s, graph, result.studentMatch, result.topicMatch, dist, path)) {
                // DFS will build path and augment it
                result.matchingSize++;
                result.augmentations++;
                phasePaths.push_back(path);
            }
        }

        result.augPathNodes.push_back(phasePaths);
    }

    return result;
}
