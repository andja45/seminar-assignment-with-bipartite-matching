#include "DemandCoverAnalysis.h"
#include <algorithm>
#include <vector>

DCAResult computeDCA(const AllocationGraph& graph, const CoverResult& cover) {
    DCAResult result;

    std::unordered_map<int, int> demand;
    for (int t : graph.getByType(NodeType::Topic))
        demand[t] = 0;

    for (const auto& pref : graph.preferences())
        demand[pref.topic]++; // how many students want that topic

    // threshold = median demand across all topics
    // median is a better choice than average — resistant to outlier topics with very high or very low demand
    std::vector<int> demandValues;
    for (int t : graph.getByType(NodeType::Topic))
        demandValues.push_back(demand[t]);
    std::sort(demandValues.begin(), demandValues.end());
    float threshold = 0.0f;
    if (!demandValues.empty()) {
        int mid = (int)demandValues.size() / 2;
        threshold = demandValues.size() % 2 == 0
            ? (demandValues[mid - 1] + demandValues[mid]) / 2.0f
            : (float)demandValues[mid];
    }

    int confirmed = 0, resolved = 0;
    for (int t : graph.getByType(NodeType::Topic)) {
        bool highDemand = demand[t] > threshold;
        bool inCover = cover.coverTopics.count(t) > 0;

        DCALabel label;
        if ( highDemand &&  inCover)      { label = DCALabel::Confirmed; confirmed++;  }
        else if ( highDemand && !inCover) { label = DCALabel::Resolved; resolved++;    }
        else if (!highDemand &&  inCover) { label = DCALabel::Structural;              }
        else                              { label = DCALabel::Clear;                   }

        result.topics[t] = { demand[t], inCover, label };
    }

    // absorptionRate = fraction of high-demand topics the matching resolved
    result.absorptionRate = (confirmed + resolved) > 0
        ? (float)resolved / (confirmed + resolved)
        : -1.0f; // -1.0 if no high-demand topics exist (index undefined)

    return result;
}
