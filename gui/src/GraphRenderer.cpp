#include "GraphRenderer.h"
#include "AppTheme.h"
#include "AppTooltips.h"
#include <cmath>
#include <set>
#include <unordered_set>
#include <algorithm>

static void dashedLine(ImDrawList* dl, ImVec2 a, ImVec2 b, ImU32 col,
                       float thick, float dash = 6.f, float gap = 4.f) {
    float dx = b.x-a.x, dy = b.y-a.y;
    float len = sqrtf(dx*dx + dy*dy);
    if (len < 0.01f) return;
    float nx = dx/len, ny = dy/len, t = 0;
    bool on = true;
    while (t < len) {
        float t1 = std::min(t + (on ? dash : gap), len);
        if (on) dl->AddLine({a.x+nx*t, a.y+ny*t}, {a.x+nx*t1, a.y+ny*t1}, col, thick);
        t = t1; on = !on;
    }
}

static ImVec2 lerp(ImVec2 a, ImVec2 b, float t) {
    return {a.x + (b.x-a.x)*t, a.y + (b.y-a.y)*t};
}

static void arrowHead(ImDrawList* dl, ImVec2 from, ImVec2 to, ImU32 col, float nodeR, float size = 16.f) {
    float dx = to.x - from.x, dy = to.y - from.y;
    float len = sqrtf(dx*dx + dy*dy);
    if (len < 1.f) return;
    float nx = dx/len, ny = dy/len, px = -ny, py = nx;
    ImVec2 tip = {to.x - nx*nodeR, to.y - ny*nodeR};
    ImVec2 base = {tip.x - nx*size, tip.y - ny*size};
    dl->AddTriangleFilled(tip,
        {base.x + px*size*0.45f, base.y + py*size*0.45f},
        {base.x - px*size*0.45f, base.y - py*size*0.45f}, col);
}

static float ptSegDist2(ImVec2 p, ImVec2 a, ImVec2 b) {
    float dx = b.x-a.x, dy = b.y-a.y, len2 = dx*dx + dy*dy;
    if (len2 < 0.01f) return (p.x-a.x)*(p.x-a.x) + (p.y-a.y)*(p.y-a.y);
    float t = std::max(0.f, std::min(1.f, ((p.x-a.x)*dx + (p.y-a.y)*dy) / len2));
    float cx = a.x+t*dx-p.x, cy = a.y+t*dy-p.y;
    return cx*cx + cy*cy;
}

void renderCanvas(AppState& state, CanvasState& canvas, ImVec2 topLeft, ImVec2 size) {
    ImGui::PushID(&canvas);
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 br = {topLeft.x + size.x, topLeft.y + size.y};

    dl->AddRectFilled(topLeft, br, AppTheme::col(AppTheme::canvasBg), 6.f);
    dl->AddRect(topLeft, br, AppTheme::canvasBorder, 6.f, 0, 1.5f);
    dl->AddText({topLeft.x+8, topLeft.y+6},
        AppTheme::canvasLabel,
        canvas.algo == AlgoChoice::Simple ? "Simple" : "Hopcroft-Karp");

    auto fmap = state.history.forbiddenMap();
    auto blocked = [&](int s, int t) {
        auto it = fmap.find(s); return it != fmap.end() && it->second.count(t);
    };

    std::unordered_set<int> animNodes;
    std::set<std::pair<int,int>> animEdges;
    std::set<std::pair<int,int>> animFwdEdges; // directed: path[i]→path[i+1]
    int astep = canvas.animStep;
    bool animActive = (canvas.animState==AnimState::Running || canvas.animState==AnimState::Paused) &&
                       astep < (int)canvas.result.augPathNodes.size();
    if (animActive) {
        for (auto& path : canvas.result.augPathNodes[astep]) {
            for (int n : path) animNodes.insert(n);
            for (int i = 0; i+1 < (int)path.size(); ++i) {
                animEdges.insert({path[i], path[i+1]});
                animEdges.insert({path[i+1], path[i]});
                animFwdEdges.insert({path[i], path[i+1]});
            }
        }
    }

    auto spos = [&](int id) -> ImVec2 {
        auto it = state.positions.find(id);
        if (it == state.positions.end()) return {topLeft.x + size.x/2, topLeft.y + size.y/2};
        return {topLeft.x + it->second.x * size.x, topLeft.y + it->second.y * size.y};
    };

    ImVec2 mouse = ImGui::GetMousePos();
    bool inCanvas = mouse.x>=topLeft.x && mouse.x<br.x && mouse.y>=topLeft.y && mouse.y<br.y;
    const float R = AppTheme::nodeRadius;

    bool flowView = state.flowView && canvas.result.matchingSize > 0;
    ImVec2 srcPos = {topLeft.x + 28.f, topLeft.y + size.y/2.f};
    ImVec2 snkPos = {br.x - 28.f, topLeft.y + size.y/2.f};
    if (flowView) {
        auto flowEdges = [&](NodeType type, ImVec2 hub, bool hubLeft) {
            const auto* mm = (type == NodeType::Student) ? &canvas.result.studentMatch : &canvas.result.topicMatch;
            for (int id : state.graph.getByType(type)) {
                ImVec2 p = spos(id);
                bool m = mm->count(id) && mm->at(id) != -1;
                ImVec2 a = hubLeft ? hub : p, b = hubLeft ? p : hub;
                dl->AddLine(a, b, m ? AppTheme::flowMatched : AppTheme::flowUnmatched, 1.5f);
                ImVec2 mid = lerp(a, b, 0.5f);
                dl->AddText({mid.x+3, mid.y-9}, AppTheme::flowLabel, m ? "1" : "0");
            }
        };
        flowEdges(NodeType::Student, srcPos, true);
        flowEdges(NodeType::Topic, snkPos, false);
    }

    for (auto& pref : state.graph.preferences()) {
        ImVec2 sp = spos(pref.student), tp = spos(pref.topic);
        bool isBlocked = blocked(pref.student, pref.topic);
        bool isMatched = canvas.result.studentMatch.count(pref.student) &&
                         canvas.result.studentMatch.at(pref.student) == pref.topic;
        bool isAnimFwd = animFwdEdges.count({pref.student, pref.topic});
        bool isAnimBwd = animFwdEdges.count({pref.topic, pref.student});
        bool isAnim    = isAnimFwd || isAnimBwd;


        if (isBlocked) {
            dashedLine(dl, sp, tp, AppTheme::col(AppTheme::edgeBlocked), 1.f);
        } else if (isAnim) {
            ImU32 ac = AppTheme::col(AppTheme::animPath);
            dl->AddLine(sp, tp, ac, AppTheme::animThick);
            arrowHead(dl, isAnimFwd ? sp : tp, isAnimFwd ? tp : sp, ac, R);
        } else if (isMatched) {
            ImU32 mc = animActive
                ? AppTheme::col(AppTheme::animDimMatch)
                : ((canvas.algo == AlgoChoice::Simple)
                    ? AppTheme::col(AppTheme::edgeMatched)
                    : AppTheme::col(AppTheme::edgeMatchedHK));
            dl->AddLine(sp, tp, mc, AppTheme::edgeMatchThick);
        } else {
            ImU32 ec = animActive
                ? AppTheme::col(AppTheme::animDimPref)
                : AppTheme::col(AppTheme::edgePref);
            dl->AddLine(sp, tp, ec, AppTheme::edgePrefThick);
        }

        if (!isBlocked) {
            ImVec2 mid = lerp(sp, tp, 0.5f);
            char rs[4]; snprintf(rs, sizeof rs, "%d", pref.rank);
            dl->AddText({mid.x+4, mid.y-10}, AppTheme::rankLabel, rs);
        }
    }

    if (state.pendingPrefFrom != -1 && inCanvas)
        dl->AddLine(spos(state.pendingPrefFrom), mouse, AppTheme::col(AppTheme::edgePref), 1.2f);

    int hoveredNode = -1;
    for (auto& [id, node] : state.graph.nodes()) {
        ImVec2 p = spos(id);
        float dx = mouse.x-p.x, dy = mouse.y-p.y;
        if (inCanvas && dx*dx+dy*dy < R*R) hoveredNode = id;
    }

    for (auto& [id, node] : state.graph.nodes()) {
        ImVec2 p = spos(id);
        bool isStu = node.type == NodeType::Student;
        bool unmatch =
            (isStu && canvas.result.studentMatch.count(id) && canvas.result.studentMatch.at(id)==-1) ||
            (!isStu && canvas.result.topicMatch.count(id) && canvas.result.topicMatch.at(id)==-1);

        bool inactive = isStu && state.inactiveStudents.count(id);
        ImVec4 fill = inactive ? AppTheme::studentInactive
                    : (isStu ? (unmatch ? AppTheme::studentDim : AppTheme::student)
                             : (unmatch ? AppTheme::topicDim : AppTheme::topic));
        dl->AddCircleFilled(p, R, AppTheme::col(fill));
        if (inactive)
            dashedLine(dl, {p.x-R*0.6f, p.y}, {p.x+R*0.6f, p.y},
                       AppTheme::inactiveStrike, 2.f, 4.f, 3.f);

        bool inCover = canvas.cover.coverStudents.count(id) || canvas.cover.coverTopics.count(id);
        if (inCover)
            dl->AddCircle(p, R, AppTheme::col(AppTheme::coverRing), 0, AppTheme::coverBorderW);

        if (animNodes.count(id))
            dl->AddCircle(p, R+3.f, AppTheme::col(AppTheme::animPath), 0, 2.5f);

        if (id == hoveredNode)
            dl->AddCircle(p, R+2.f, AppTheme::hoverRing, 0, 1.5f);

        ImVec2 ts = ImGui::CalcTextSize(node.label.c_str());
        dl->AddText({p.x-ts.x/2, p.y-ts.y/2}, AppTheme::col(AppTheme::nodeText), node.label.c_str());
    }

    if (flowView) {
        dl->AddCircleFilled(srcPos, 14.f, AppTheme::col(AppTheme::flowSrcSink));
        dl->AddCircleFilled(snkPos, 14.f, AppTheme::col(AppTheme::flowSrcSink));
        auto ss = ImGui::CalcTextSize("S");
        dl->AddText({srcPos.x-ss.x/2, srcPos.y-ss.y/2}, AppTheme::white, "S");
        dl->AddText({snkPos.x-ss.x/2, snkPos.y-ss.y/2}, AppTheme::white, "T");
    }

    if (animActive) {
        int total = (int)canvas.result.augPathNodes.size();
        int nPaths = (int)canvas.result.augPathNodes[astep].size();
        char stepLabel[64];
        if (canvas.algo == AlgoChoice::HopcroftKarp)
            snprintf(stepLabel, sizeof stepLabel, "Phase %d / %d  (%d path%s)",
                     astep+1, total, nPaths, nPaths==1?"":"s");
        else
            snprintf(stepLabel, sizeof stepLabel, "Augmentation %d / %d", astep+1, total);
        ImVec2 ts = ImGui::CalcTextSize(stepLabel);
        dl->AddText({topLeft.x + size.x/2.f - ts.x/2.f, topLeft.y + 8.f},
                    AppTheme::col(AppTheme::animPath), stepLabel);
    }

    if (!inCanvas) { ImGui::PopID(); return; }

    if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && hoveredNode != -1 &&
        state.graph.nodes().at(hoveredNode).type == NodeType::Student) {
        if (state.inactiveStudents.count(hoveredNode))
            state.inactiveStudents.erase(hoveredNode);
        else
            state.inactiveStudents.insert(hoveredNode);
        if (state.pendingPrefFrom == hoveredNode) state.pendingPrefFrom = -1;
    }
    else if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
        if (state.placingType && hoveredNode == -1) {
            NodeType t = *state.placingType;
            std::string lbl = (t==NodeType::Student?"S":"T") +
                              std::to_string(state.graph.getByType(t).size()+1);
            int id = state.graph.addNode(lbl, t);
            state.positions[id] = {(mouse.x - topLeft.x) / size.x,
                                   (mouse.y - topLeft.y) / size.y};
        } else {
            state.placingType = std::nullopt;

            if (state.pendingPrefFrom != -1) {
                if (hoveredNode != -1) {
                    auto& hn = state.graph.nodes().at(hoveredNode);
                    if (hn.type == NodeType::Topic) {
                        int nextRank = (int)state.graph.prefsOf(state.pendingPrefFrom).size() + 1;
                        if (nextRank <= state.maxPrefsPerStudent)
                            state.graph.addPreference(state.pendingPrefFrom, hoveredNode, nextRank);
                        state.pendingPrefFrom = -1;
                    } else {
                        state.pendingPrefFrom = hoveredNode;
                    }
                } else {
                    state.pendingPrefFrom = -1;
                }
            } else {
                if (hoveredNode != -1) {
                    if (state.graph.nodes().at(hoveredNode).type == NodeType::Student)
                        state.pendingPrefFrom = hoveredNode;
                    state.draggedNode = hoveredNode;
                }
            }
        }
    }

    if (state.draggedNode != -1 && ImGui::IsMouseDragging(ImGuiMouseButton_Left, 4.f)) {
        state.positions[state.draggedNode] = {(mouse.x - topLeft.x) / size.x,
                                              (mouse.y - topLeft.y) / size.y};
        state.pendingPrefFrom = -1;
    }

    if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
        state.draggedNode = -1;

    if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
        bool graphChanged = false;
        if (hoveredNode != -1 && state.graph.hasNode(hoveredNode)) {
            if (state.pendingPrefFrom == hoveredNode) state.pendingPrefFrom = -1;
            state.positions.erase(hoveredNode);
            state.graph.removeNode(hoveredNode);
            graphChanged = true;
        } else {
            for (auto& pref : state.graph.preferences()) {
                if (ptSegDist2(mouse, spos(pref.student), spos(pref.topic)) < 36.f) {
                    state.graph.removePreference(pref.student, pref.topic);
                    graphChanged = true;
                    break;
                }
            }
        }
        if (graphChanged) state.clearResults();
    }

    if (hoveredNode == -1 || !state.graph.hasNode(hoveredNode)) { ImGui::PopID(); return; }
    auto& hn = state.graph.nodes().at(hoveredNode);
    bool isStu = hn.type == NodeType::Student;

    ImGui::BeginTooltip();
    ImGui::PushTextWrapPos(280.f);
    ImGui::TextColored(isStu ? AppTheme::student : AppTheme::topic, "%s", hn.label.c_str());
    ImGui::Separator();

    if (!canvas.result.studentMatch.empty()) {
        if (isStu) {
            auto it = canvas.result.studentMatch.find(hoveredNode);
            if (it != canvas.result.studentMatch.end() && it->second != -1) {
                int tid = it->second, rank = 0;
                for (auto& p : state.graph.preferences())
                    if (p.student==hoveredNode && p.topic==tid) { rank=p.rank; break; }
                ImGui::Text("Matched: %s  (rank %d)", state.graph.get(tid).label.c_str(), rank);
            } else {
                ImGui::TextColored(AppTheme::textBad, "Unmatched");
            }
        } else {
            auto it = canvas.result.topicMatch.find(hoveredNode);
            if (it != canvas.result.topicMatch.end() && it->second != -1)
                ImGui::Text("Assigned: %s", state.graph.get(it->second).label.c_str());
            else
                ImGui::TextColored(AppTheme::textMuted, "Free");
        }
    }

    if (isStu) {
        auto fit = canvas.flexibility.studentLevel.find(hoveredNode);
        if (fit != canvas.flexibility.studentLevel.end()) {
            bool forced = fit->second == FlexibilityLevel::Forced;
            ImGui::TextColored(forced ? AppTheme::textBad : AppTheme::textGood,
                forced ? "Forced assignment" : "Flexible assignment");
            if (!forced) {
                auto alt = canvas.flexibility.alternativeTopics.find(hoveredNode);
                if (alt != canvas.flexibility.alternativeTopics.end()) {
                    std::string alts;
                    for (int tid : alt->second) alts += state.graph.get(tid).label + " ";
                    ImGui::TextColored(AppTheme::textMuted, "Alt: %s", alts.c_str());
                }
            }
        }
        auto rit = canvas.relief.perStudent.find(hoveredNode);
        if (rit != canvas.relief.perStudent.end() && !rit->second.empty()) {
            std::string rs;
            for (int tid : rit->second) rs += state.graph.get(tid).label + " ";
            ImGui::TextColored(AppTheme::textWarn, "Relief: %s", rs.c_str());
        }
    } else {
        auto dit = canvas.dca.topics.find(hoveredNode);
        if (dit != canvas.dca.topics.end() && dit->second.label != DCALabel::Clear) {
            const char* lbl =
                dit->second.label==DCALabel::Confirmed ? "Confirmed bottleneck" :
                dit->second.label==DCALabel::Resolved ? "Resolved" : "Structural";
            ImVec4 c =
                dit->second.label==DCALabel::Confirmed ? AppTheme::textBad :
                dit->second.label==DCALabel::Resolved ? AppTheme::textGood : AppTheme::textWarn;
            ImGui::TextColored(c, "%s  (demand: %d)", lbl, dit->second.demand);
        }
    }

    auto hit = state.history.assignments.find(hoveredNode);
    if (isStu && hit != state.history.assignments.end() && !hit->second.empty()) {
        ImGui::Separator();
        const char* pn[] = {"Jan1","Jan2","Jun1","Jun2","Sep1","Sep2"};
        for (auto& [rnd, tid] : hit->second)
            ImGui::TextColored(AppTheme::textMuted, "  %s: %s",
                pn[(int)rnd], state.graph.hasNode(tid) ? state.graph.get(tid).label.c_str() : "?");
    }

    ImGui::PopTextWrapPos();
    ImGui::EndTooltip();

    ImGui::PopID();
}
