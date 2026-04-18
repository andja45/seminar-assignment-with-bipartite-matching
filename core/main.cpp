#include <iostream>
#include <iomanip>
#include "examples/AllocationExamples.h"
#include "matching/SimpleMatching.h"
#include "matching/HopcroftKarp.h"
#include "cover/KonigCover.h"
#include "cover/DemandCoverAnalysis.h"
#include "analysis/ReliefAdvisor.h"
#include "analysis/MatchingFlexibility.h"

static void printMatching(const std::string& algo, const MatchingResult& m,
                          const AllocationGraph& g) {
    std::cout << "  [" << algo << "] matched=" << m.matchingSize
              << "  augmentations=" << m.augmentations
              << "  phases=" << m.phases
              << "  edge traversals=" << m.edgeTraversals << "\n";
    for (auto& [s, t] : m.studentMatch) {
        if (t == -1) continue;
        std::cout << "    " << g.get(s).label << " -> " << g.get(t).label << "\n";
    }
    for (auto& [s, t] : m.studentMatch)
        if (t == -1) std::cout << "    " << g.get(s).label << " -> (unmatched)\n";
}

static void printCoverAndDCA(const AllocationGraph& g,
                              const CoverResult& cover, const DCAResult& dca) {
    std::cout << "  Cover size=" << (cover.coverStudents.size() + cover.coverTopics.size()) << "\n";
    std::cout << "  DCA absorption rate=";
    if (dca.absorptionRate < 0) std::cout << "n/a (no high-demand topics)\n";
    else std::cout << std::fixed << std::setprecision(2) << dca.absorptionRate << "\n";

    for (auto& [id, tdca] : dca.topics) {
        if (tdca.label == DCALabel::Clear) continue;
        std::string label;
        switch (tdca.label) {
            case DCALabel::Confirmed:  label = "Confirmed";  break;
            case DCALabel::Resolved:   label = "Resolved";   break;
            case DCALabel::Structural: label = "Structural"; break;
            default: break;
        }
        std::cout << "    " << g.get(id).label
                  << " demand=" << tdca.demand
                  << " inCover=" << tdca.inCover
                  << " -> " << label << "\n";
    }
}

static void printRelief(const AllocationGraph& g, const ReliefResult& relief,
                        const MatchingResult& matching) {
    bool anyUnmatched = false;
    for (auto& [s, t] : matching.studentMatch)
        if (t == -1) { anyUnmatched = true; break; }

    if (!anyUnmatched) { std::cout << "  Relief: all students matched\n"; return; }
    if (relief.perStudent.empty()) {
        std::cout << "  Relief: no topic expansion can unblock unmatched students\n"; return;
    }
    for (auto& [s, topics] : relief.perStudent) {
        std::cout << "  Relief for " << g.get(s).label << ": ";
        for (int t : topics) std::cout << g.get(t).label << " ";
        std::cout << "\n";
    }
    std::cout << "  Topic gain: ";
    for (auto& [t, gain] : relief.topicGain)
        std::cout << g.get(t).label << "=" << gain << " ";
    std::cout << "\n";
}

static void printFlexibility(const AllocationGraph& g, const MatchingFlexibilityResult& flex) {
    std::cout << "  Flexibility index=" << std::fixed << std::setprecision(2)
              << flex.flexibilityIndex << " (forced/matched; 1.0=rigid)\n";
    for (auto& [s, level] : flex.studentLevel) {
        std::string tag = (level == FlexibilityLevel::Forced) ? "Forced" : "Flexible";
        std::cout << "    " << g.get(s).label << ": " << tag;
        if (level == FlexibilityLevel::Flexible) {
            auto it = flex.alternativeTopics.find(s);
            if (it != flex.alternativeTopics.end() && !it->second.empty())
                std::cout << " (alt: " << g.get(it->second.front()).label << ")";
        }
        std::cout << "\n";
    }
}

static void runSingleRound(Example& ex) {
    AllocationGraph& g = ex.graph;

    MatchingResult simple = runSimpleMatching(g);
    MatchingResult hk     = runHopcroftKarp(g);
    printMatching("Simple", simple, g);
    printMatching("HopcorftKarp", hk,     g);

    CoverResult cover = computeCover(g, hk);
    DCAResult   dca   = computeDCA(g, cover);
    printCoverAndDCA(g, cover, dca);

    ReliefResult relief = computeRelief(g, hk);
    printRelief(g, relief, hk);

    MatchingFlexibilityResult flex = computeFlexibility(g, hk);
    printFlexibility(g, flex);
}

static void runCohortRotation(Example& ex) {
    AllocationGraph& g = ex.graph;

    // Round 1 was already committed in buildCohortRotation(); print it
    std::cout << "  *** Round 1 (Jan1) ***\n";
    AllocationGraph r1graph = g.forRound({});
    MatchingResult r1 = runSimpleMatching(r1graph);
    printMatching("Simple", r1, r1graph);

    // Round 2: filter out topics each student already received in round 1
    std::cout << "  *** Round 2 (Jan2) ***\n";
    AllocationGraph r2graph = g.forRound(ex.history.forbiddenMap());
    MatchingResult r2 = runSimpleMatching(r2graph);
    printMatching("Simple", r2, r2graph);

    // Unmatched rounds per student - no student should be unmatched more than others
    ex.history.commit(ExamRound::Jan2, r2);
    std::cout << "  Fairness (unmatched rounds):\n";
    for (int s : g.getByType(NodeType::Student))
        std::cout << "    " << g.get(s).label
                  << ": " << ex.history.unmatchedRounds(s) << " unmatched\n";
}

int main() {
    {
        auto ex = buildBottleneckRound();
        std::cout << "\n" << ex.name << "\n";
        runSingleRound(ex);
    }
    {
        auto ex = buildLayeredPhases();
        std::cout << "\n" << ex.name << "\n";
        runSingleRound(ex);
    }
    {
        auto ex = buildRoundFairness();
        std::cout << "\n" << ex.name << "\n";
        runCohortRotation(ex);
    }
    return 0;
}
