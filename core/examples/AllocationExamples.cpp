#include "AllocationExamples.h"
#include "matching/SimpleMatching.h"

Example buildBottleneckRound() {
    Example ex;
    ex.name = "Bottleneck Round";
    AllocationGraph& g = ex.graph;

    int s1 = g.addNode("S1", NodeType::Student);
    int s2 = g.addNode("S2", NodeType::Student);
    int s3 = g.addNode("S3", NodeType::Student);
    int s4 = g.addNode("S4", NodeType::Student);
    int s5 = g.addNode("S5", NodeType::Student);
    int s6 = g.addNode("S6", NodeType::Student);
    int s7 = g.addNode("S7", NodeType::Student);
    int s8 = g.addNode("S8", NodeType::Student);

    int t1 = g.addNode("T1", NodeType::Topic);
    int t2 = g.addNode("T2", NodeType::Topic);
    int t3 = g.addNode("T3", NodeType::Topic);
    int t4 = g.addNode("T4", NodeType::Topic);
    int t5 = g.addNode("T5", NodeType::Topic);
    g.addNode("T6", NodeType::Topic); // niche topic — no student lists it, stays free

    // S1–S4 all want T1 first → bottleneck
    g.addPreference(s1, t1, 1); g.addPreference(s1, t2, 2); g.addPreference(s1, t3, 3);
    g.addPreference(s2, t1, 1); g.addPreference(s2, t2, 2); g.addPreference(s2, t4, 3);
    g.addPreference(s3, t1, 1); g.addPreference(s3, t3, 2); g.addPreference(s3, t5, 3);
    g.addPreference(s4, t1, 1); g.addPreference(s4, t4, 2);
    // S5–S6 compete on T2
    g.addPreference(s5, t2, 1); g.addPreference(s5, t3, 2);
    g.addPreference(s6, t2, 1); g.addPreference(s6, t5, 2);
    // S7–S8 cover lower-demand topics
    g.addPreference(s7, t3, 1); g.addPreference(s7, t4, 2);
    g.addPreference(s8, t4, 1); g.addPreference(s8, t5, 2);

    return ex;
}

Example buildLayeredPhases() {
    Example ex;
    ex.name = "Layered Phases";
    AllocationGraph& g = ex.graph;

    int s1 = g.addNode("S1", NodeType::Student);
    int s2 = g.addNode("S2", NodeType::Student);
    int s3 = g.addNode("S3", NodeType::Student);
    int s4 = g.addNode("S4", NodeType::Student);
    int s5 = g.addNode("S5", NodeType::Student);
    int s6 = g.addNode("S6", NodeType::Student);
    int s7 = g.addNode("S7", NodeType::Student);

    int t1 = g.addNode("T1", NodeType::Topic);
    int t2 = g.addNode("T2", NodeType::Topic);
    int t3 = g.addNode("T3", NodeType::Topic);
    int t4 = g.addNode("T4", NodeType::Topic);
    int t5 = g.addNode("T5", NodeType::Topic);

    g.addPreference(s1, t1, 1); g.addPreference(s1, t2, 2);
    g.addPreference(s2, t1, 1); g.addPreference(s2, t3, 2);
    g.addPreference(s3, t2, 1); g.addPreference(s3, t4, 2);
    g.addPreference(s4, t3, 1); g.addPreference(s4, t5, 2);
    g.addPreference(s5, t4, 1); g.addPreference(s5, t5, 2);
    g.addPreference(s6, t1, 1); // only one preference → unmatched
    g.addPreference(s7, t2, 1); // only one preference → unmatched

    return ex;
}

Example buildRoundFairness() {
    Example ex;
    ex.name = "Round Fairness";
    AllocationGraph& g = ex.graph;

    int s1 = g.addNode("S1", NodeType::Student);
    int s2 = g.addNode("S2", NodeType::Student);
    int s3 = g.addNode("S3", NodeType::Student);
    int s4 = g.addNode("S4", NodeType::Student);

    int t1 = g.addNode("T1", NodeType::Topic);
    g.addNode("T2", NodeType::Topic); // in graph but no student prefers it in round 1
    int t3 = g.addNode("T3", NodeType::Topic);
    g.addNode("T4", NodeType::Topic); // same

    // S1 and S2 both only want T1 — one gets it, the other is left unmatched
    // S3 and S4 both only want T3 — same conflict on the other side
    g.addPreference(s1, t1, 1);
    g.addPreference(s2, t1, 1);
    g.addPreference(s3, t3, 1);
    g.addPreference(s4, t3, 1);

    // run round 1 and record it in history so main.cpp can call forRound() for round 2
    MatchingResult r1 = runSimpleMatching(g);
    ex.history.commit(ExamRound::Jan1, r1);

    return ex;
}
