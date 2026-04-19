#include "AppState.h"
#include "matching/SimpleMatching.h"
#include "matching/HopcroftKarp.h"
#include "cover/KonigCover.h"
#include "cover/DemandCoverAnalysis.h"
#include "analysis/ReliefAdvisor.h"
#include "analysis/MatchingFlexibility.h"
#include "examples/AllocationExamples.h"
#include <algorithm>

AllocationGraph AppState::activeGraph() const {
    AllocationGraph g = graph.forRound(history.forbiddenMap());
    for (int sid : inactiveStudents) g.removeNode(sid);
    return g;
}

void AppState::runMatching() {
    auto g = activeGraph();
    auto runAlgo = [&](CanvasState& c) {
        c.result = (c.algo == AlgoChoice::Simple) ? runSimpleMatching(g) : runHopcroftKarp(g);
        c.animState = AnimState::Running;
        c.animStep = 0;
    };
    runAlgo(topCanvas); runAlgo(botCanvas);
    timeSinceStep = 0;
}

void AppState::runCover() {
    auto g = activeGraph();
    auto runOne = [&](CanvasState& c) { c.cover = computeCover(g, c.result); c.dca = computeDCA(g, c.cover); };
    runOne(topCanvas); runOne(botCanvas);
}

void AppState::runAnalysis() {
    auto g = activeGraph();
    topCanvas.relief = computeRelief(g, topCanvas.result);
    topCanvas.flexibility = computeFlexibility(g, topCanvas.result);
}

void AppState::loadExample(int which) {
    clearAll();
    Example ex = (which == 0) ? buildBottleneckRound()
               : (which == 1) ? buildLayeredPhases()
               :                buildRoundFairness();

    graph = std::move(ex.graph);
    history = std::move(ex.history);
    period = (which == 2) ? ExamRound::Jan2 : ExamRound::Jan1;

    for (auto& [id, node] : graph.nodes())
        autoPlaceNode(id, node.type, 1.f, 1.f);
}

void AppState::clearAll() {
    graph = {};
    history = {};
    period = ExamRound::Jan1;
    flowView = false;
    splitView = false;
    placingType = std::nullopt;
    pendingPrefFrom = -1;
    draggedNode = -1;
    inactiveStudents.clear();
    timeSinceStep = 0;
    positions.clear();
    topCanvas = {};
    botCanvas = {};
}

void AppState::tickAnimation(float dt) {
    timeSinceStep += dt;
    if (timeSinceStep < stepDelay) return;
    timeSinceStep = 0;

    auto advance = [](CanvasState& c) {
        if (c.animState != AnimState::Running) return;
        if (++c.animStep >= (int)c.result.augPathNodes.size())
            c.animState = AnimState::Done;
    };
    advance(topCanvas); advance(botCanvas);
}

void AppState::autoPlaceNode(int id, NodeType type, float /*canvasW*/, float /*canvasH*/) {
    auto byType = graph.getByType(type);
    int count = (int)byType.size();
    int idx = 0;
    for (int i = 0; i < count; ++i)
        if (byType[i] == id) { idx = i; break; }

    const float spacing = 0.11f;
    float totalH = (count - 1) * spacing;
    float startY = (1.f - totalH) / 2.f;

    float x = (type == NodeType::Student) ? 0.28f : 0.72f;
    positions[id] = {x, startY + idx * spacing};
}
