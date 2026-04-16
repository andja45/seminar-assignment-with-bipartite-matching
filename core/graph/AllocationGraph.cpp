#include "AllocationGraph.h"
#include <algorithm>

void AllocationGraph::rebuildPrefsOf(int student) {
    std::vector<std::pair<int,int>> ranked; // (rank, topicId)
    for (const auto& p : m_prefs)
        if (p.student == student) ranked.push_back({p.rank, p.topic});
    std::sort(ranked.begin(), ranked.end());
    auto& v = m_prefsByStudent[student];
    v.clear();
    for (const auto& [r, t] : ranked) v.push_back(t);
}

int AllocationGraph::addNode(const std::string& label, NodeType type) {
    int id = m_nextId++;
    m_nodes[id] = {id, label, type};
    if (type == NodeType::Student) m_prefsByStudent[id] = {};
    return id;
}

bool AllocationGraph::addPreference(int student, int topic, int rank) {
    if (!hasNode(student) || !hasNode(topic)) return false;
    if (m_nodes.at(student).type != NodeType::Student) return false;
    if (m_nodes.at(topic).type != NodeType::Topic) return false;
    for (const auto& p : m_prefs)
        if (p.student == student && p.topic == topic) return false;
    m_prefs.push_back({student, topic, rank});
    rebuildPrefsOf(student);
    return true;
}

bool AllocationGraph::removePreference(int student, int topic) {
    auto it = std::find_if(m_prefs.begin(), m_prefs.end(),
        [&](const Preference& p) { return p.student == student && p.topic == topic; });
    if (it == m_prefs.end()) return false;
    m_prefs.erase(it);
    rebuildPrefsOf(student);
    return true;
}

bool AllocationGraph::removeNode(int id) {
    if (!hasNode(id)) return false;
    NodeType type = m_nodes.at(id).type;
    if (type == NodeType::Student) {
        m_prefs.erase(
            std::remove_if(m_prefs.begin(), m_prefs.end(),
                [id](const Preference& p) { return p.student == id; }),
            m_prefs.end());
        m_prefsByStudent.erase(id);
    } else {
        m_prefs.erase(
            std::remove_if(m_prefs.begin(), m_prefs.end(),
                [id](const Preference& p) { return p.topic == id; }),
            m_prefs.end());
        for (auto& [sid, _] : m_prefsByStudent) rebuildPrefsOf(sid);
    }
    m_nodes.erase(id);
    return true;
}

bool AllocationGraph::hasNode(int id) const {
    return m_nodes.count(id) > 0;
}

const Node& AllocationGraph::get(int id) const {
    return m_nodes.at(id);
}

const std::unordered_map<int, Node>& AllocationGraph::nodes() const {
    return m_nodes;
}

const std::vector<Preference>& AllocationGraph::preferences() const {
    return m_prefs;
}

const std::vector<int>& AllocationGraph::prefsOf(int studentId) const {
    static const std::vector<int> empty;
    auto it = m_prefsByStudent.find(studentId);
    if (it == m_prefsByStudent.end()) return empty;
    return it->second;
}

std::vector<int> AllocationGraph::getByType(NodeType type) const {
    std::vector<int> result;
    for (const auto& [id, node] : m_nodes)
        if (node.type == type) result.push_back(id);
    return result;
}

AllocationGraph AllocationGraph::forRound(
    const std::unordered_map<int, std::unordered_set<int>>& forbidden) const {
    AllocationGraph g;
    g.m_nextId = m_nextId;
    g.m_nodes = m_nodes;
    for (const auto& [id, node] : m_nodes)
        if (node.type == NodeType::Student) g.m_prefsByStudent[id] = {};
    for (const auto& p : m_prefs) {
        auto it = forbidden.find(p.student);
        if (it != forbidden.end() && it->second.count(p.topic)) continue; // skip (student, forbidden topic)
        g.m_prefs.push_back(p);
    }
    for (const auto& [sid, _] : g.m_prefsByStudent) g.rebuildPrefsOf(sid);
    return g;
}
