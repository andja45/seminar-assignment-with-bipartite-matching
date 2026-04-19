#ifndef SEMINARALLOCATIONENGINE_APPSTATE_H
#define SEMINARALLOCATIONENGINE_APPSTATE_H

#include <optional>
#include <unordered_map>
#include <unordered_set>
#include "graph/AllocationGraph.h"
#include "rounds/RoundHistory.h"
#include "matching/MatchingResult.h"
#include "cover/KonigCover.h"
#include "cover/DemandCoverAnalysis.h"
#include "analysis/ReliefAdvisor.h"
#include "analysis/MatchingFlexibility.h"

enum class AlgoChoice { Simple, HopcroftKarp };

enum class AnimState { Idle, Running, Paused, Done };

struct CanvasState {
    AlgoChoice algo = AlgoChoice::Simple;

    MatchingResult result;
    CoverResult cover;
    DCAResult dca;
    ReliefResult relief;
    MatchingFlexibilityResult flexibility;
    RoundHistory history;

    AnimState animState = AnimState::Idle;
    int animStep = 0;
};

struct NodePos { float x = 0, y = 0; };

struct AppState {
    AllocationGraph graph;
    RoundHistory history;
    ExamRound period = ExamRound::Jan1;
    bool flowView = false;
    bool splitView = false;

    std::optional<NodeType> placingType;
    int pendingPrefFrom = -1;
    int maxPrefsPerStudent = 3;

    std::unordered_set<int> inactiveStudents;

    std::unordered_map<int, NodePos> positions;
    int draggedNode = -1;

    float stepDelay = 0.5f;
    float timeSinceStep = 0.0f;

    CanvasState topCanvas;
    CanvasState botCanvas;

    void runMatching();
    void runCover();
    void runAnalysis();
    void loadExample(int which);
    void clearAll();
    void tickAnimation(float dt);
    void autoPlaceNode(int id, NodeType type, float canvasW, float canvasH);

private:
    AllocationGraph activeGraph() const;
};

#endif
