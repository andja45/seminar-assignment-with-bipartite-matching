#ifndef SEMINARALLOCATIONENGINE_ALLOCATIONGRAPH_H
#define SEMINARALLOCATIONENGINE_ALLOCATIONGRAPH_H

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "Node.h"
#include "Preference.h"

class AllocationGraph {
    int m_nextId = 0;
    std::unordered_map<int, Node> m_nodes;
    std::vector<Preference> m_prefs;
    std::unordered_map<int, std::vector<int>> m_prefsByStudent; // sorted by rank

    void rebuildPrefsOf(int student);
public:
    int addNode(const std::string& label, NodeType type);
    bool addPreference(int student, int topic, int rank);
    bool removePreference(int student, int topic);
    bool removeNode(int id);

    bool hasNode(int id) const;
    const Node& get(int id) const;
    const std::unordered_map<int, Node>& nodes() const;
    const std::vector<Preference>& preferences() const;
    const std::vector<int>& prefsOf(int studentId) const; // sorted by rank
    std::vector<int> getByType(NodeType type) const;

    AllocationGraph forRound( // graph with removed topics that student cannot take in this round
        const std::unordered_map<int, std::unordered_set<int>>& forbidden) const;
};

#endif
