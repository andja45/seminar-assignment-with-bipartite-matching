#include "App.h"
#include "AppTheme.h"
#include "AppTooltips.h"
#include "imgui.h"
#include <algorithm>
#include <string>

static void coloredText(const char* t, ImVec4 c) { ImGui::TextColored(c, "%s", t); }

static bool primaryButton(const char* label, float w = 0) {
    ImGui::PushStyleColor(ImGuiCol_Button, AppTheme::col(AppTheme::btnPrimary));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, AppTheme::col(AppTheme::btnPrimaryHover));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, AppTheme::col(AppTheme::btnPrimaryActive));
    bool v = ImGui::Button(label, {w, 0});
    ImGui::PopStyleColor(3);
    return v;
}

static bool redButton(const char* label, float w = 0) {
    ImGui::PushStyleColor(ImGuiCol_Button, AppTheme::col(AppTheme::btnRed));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, AppTheme::col(AppTheme::btnRedHover));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, AppTheme::col(AppTheme::btnRedActive));
    bool v = ImGui::Button(label, {w, 0});
    ImGui::PopStyleColor(3);
    return v;
}

void App::leftPanel() {
    float bw = ImGui::GetContentRegionAvail().x;

    ImGui::TextColored(AppTheme::textMuted, "Place node");
    bool placingS = m_state.placingType && *m_state.placingType == NodeType::Student;
    bool placingT = m_state.placingType && *m_state.placingType == NodeType::Topic;

    if (placingS) ImGui::PushStyleColor(ImGuiCol_Button, AppTheme::col(AppTheme::student));
    if (ImGui::Button("Student", {bw/2.f - 2.f, 0})) {
        m_state.placingType = placingS ? std::nullopt : std::optional(NodeType::Student);
        m_state.pendingPrefFrom = -1;
    }
    if (placingS) ImGui::PopStyleColor();
    ImGui::SameLine(0, 4.f);
    if (placingT) ImGui::PushStyleColor(ImGuiCol_Button, AppTheme::col(AppTheme::topic));
    if (ImGui::Button("Topic", {bw/2.f - 2.f, 0})) {
        m_state.placingType = placingT ? std::nullopt : std::optional(NodeType::Topic);
        m_state.pendingPrefFrom = -1;
    }
    if (placingT) ImGui::PopStyleColor();

    ImGui::Separator();

    ImGui::SetNextItemWidth(72.f);
    ImGui::InputInt("Max prefs##m", &m_state.maxPrefsPerStudent);
    m_state.maxPrefsPerStudent = std::clamp(m_state.maxPrefsPerStudent, 1, 10);
    Tooltips::ifHovered(Tooltips::maxPrefs);

    ImGui::Separator();

    const char* roundNames[] = {"Jan 1", "Jan 2", "Jun 1", "Jun 2", "Sep 1", "Sep 2"};
    int p = (int)m_state.period;
    ImGui::SetNextItemWidth(-1);
    if (ImGui::Combo("##period", &p, roundNames, 6)) m_state.period = (ExamRound)p;
    ImGui::Checkbox("Split view", &m_state.splitView);

    auto algoRadio = [](const char* idS, const char* idHK, AlgoChoice& algo) {
        bool s = algo == AlgoChoice::Simple;
        bool hk = algo == AlgoChoice::HopcroftKarp;
        if (ImGui::RadioButton(idS, s)) algo = AlgoChoice::Simple;
        ImGui::SameLine();
        if (ImGui::RadioButton(idHK, hk)) algo = AlgoChoice::HopcroftKarp;
    };
    if (m_state.splitView) {
        ImGui::TextColored(AppTheme::textMuted, "Left");
        ImGui::SameLine();
        algoRadio("Simple##tl", "HK##tl", m_state.topCanvas.algo);
        ImGui::TextColored(AppTheme::textMuted, "Right");
        ImGui::SameLine();
        algoRadio("Simple##bl", "HK##bl", m_state.botCanvas.algo);
    } else {
        algoRadio("Simple##a", "HK##a", m_state.topCanvas.algo);
    }

    ImGui::Separator();

    if (primaryButton("Run Matching", bw)) m_state.runMatching();
    Tooltips::ifHovered(Tooltips::matching);
    if (primaryButton("Run Cover", bw)) m_state.runCover();
    Tooltips::ifHovered(Tooltips::cover);
    if (primaryButton("Run Analysis", bw)) m_state.runAnalysis();
    Tooltips::ifHovered(Tooltips::analysis);

    {
        bool hasResult = !m_state.topCanvas.result.studentMatch.empty();
        if (!hasResult) ImGui::BeginDisabled();
        if (primaryButton("Commit Round", bw)) {
            m_state.history.commit(m_state.period, m_state.topCanvas.result);
            m_state.topCanvas.history.commit(m_state.period, m_state.topCanvas.result);
            if (m_state.splitView && !m_state.botCanvas.result.studentMatch.empty())
                m_state.botCanvas.history.commit(m_state.period, m_state.botCanvas.result);
            int next = (int)m_state.period + 1;
            if (next < 6) m_state.period = (ExamRound)next;
        }
        Tooltips::ifHovered(Tooltips::commitRound);
        if (!hasResult) ImGui::EndDisabled();
    }

    ImGui::Separator();

    auto& tc = m_state.topCanvas;
    if (tc.animState == AnimState::Running || tc.animState == AnimState::Paused) {
        bool running = tc.animState == AnimState::Running;
        if (ImGui::Button(running ? "Pause##a" : "Resume##a", {bw * 0.48f, 0})) {
            auto toggle = [](CanvasState& c) {
                if (c.animState == AnimState::Running) c.animState = AnimState::Paused;
                else if (c.animState == AnimState::Paused) c.animState = AnimState::Running;
            };
            toggle(m_state.topCanvas); toggle(m_state.botCanvas);
        }
        ImGui::SameLine();
    }
    ImGui::SetNextItemWidth(-1.f);
    ImGui::SliderFloat("##speed", &m_state.stepDelay, 0.05f, 2.0f, "%.2fs");
    Tooltips::ifHovered(Tooltips::animSpeed);

    ImGui::Checkbox("Flow view", &m_state.flowView);
    Tooltips::ifHovered(Tooltips::viewFlow);

    ImGui::Separator();

    coloredText("Examples", AppTheme::textMuted);
    if (ImGui::Button("Bottleneck Round", {bw, 0})) m_state.loadExample(0);
    if (ImGui::Button("Layered Phases", {bw, 0})) m_state.loadExample(1);
    if (ImGui::Button("Round Fairness", {bw, 0})) m_state.loadExample(2);

    ImGui::Separator();
    if (redButton("Clear All", bw)) m_state.clearAll();
}

static void showMatchingSection(AppState& state, CanvasState& c, const char* label) {
    if (c.result.studentMatch.empty()) {
        coloredText("No results yet.", AppTheme::textMuted);
        return;
    }
    ImGui::BulletText("%d/%zu matched", c.result.matchingSize,
        state.graph.getByType(NodeType::Student).size());
    ImGui::BulletText("Augmentations: %d", c.result.augmentations);
    ImGui::BulletText("Phases: %d", c.result.phases);
    ImGui::BulletText("Edge traversals: %d", c.result.edgeTraversals);

    if (ImGui::TreeNode(label)) {
        for (auto& [sid, tid] : c.result.studentMatch) {
            if (tid == -1) {
                ImGui::TextColored(AppTheme::textBad, "  %s -> unmatched",
                    state.graph.get(sid).label.c_str());
                continue;
            }
            int rank = 0;
            for (auto& pref : state.graph.preferences())
                if (pref.student == sid && pref.topic == tid) { rank = pref.rank; break; }
            ImGui::Text("  %s -> %s  (rank %d)",
                state.graph.get(sid).label.c_str(),
                state.graph.get(tid).label.c_str(), rank);
        }
        ImGui::TreePop();
    }
}

static void showCoverSection(AppState& state, CanvasState& c) {
    if (c.cover.coverStudents.empty() && c.cover.coverTopics.empty()) {
        coloredText("Run Cover first.", AppTheme::textMuted); return;
    }
    size_t sz = c.cover.coverStudents.size() + c.cover.coverTopics.size();
    ImGui::BulletText("Cover size: %zu", sz);
    if (c.dca.absorptionRate >= 0)
        ImGui::BulletText("Absorption rate: %.0f%%", c.dca.absorptionRate * 100.f);

    for (auto& [tid, dca] : c.dca.topics) {
        if (dca.label == DCALabel::Clear) continue;
        const char* lbl =
            dca.label == DCALabel::Confirmed ? "Confirmed" :
            dca.label == DCALabel::Resolved ? "Resolved" : "Structural";
        ImVec4 col =
            dca.label == DCALabel::Confirmed ? AppTheme::textBad :
            dca.label == DCALabel::Resolved ? AppTheme::textGood : AppTheme::textWarn;
        ImGui::TextColored(col, "  %s  %s  (demand %d)",
            state.graph.get(tid).label.c_str(), lbl, dca.demand);
        Tooltips::ifHovered(
            dca.label == DCALabel::Confirmed ? Tooltips::dcaConfirmed :
            dca.label == DCALabel::Resolved ? Tooltips::dcaResolved : Tooltips::dcaStructural);
    }
    for (int sid : c.cover.coverStudents)
        ImGui::Text("  %s  (cover - student side)", state.graph.get(sid).label.c_str());
}

static void showReliefSection(AppState& state, CanvasState& c) {
    if (c.relief.perStudent.empty()) {
        bool hasUnmatched = false;
        for (auto& [sid, tid] : c.result.studentMatch) hasUnmatched |= (tid == -1);
        coloredText(hasUnmatched ? "No relief topics found." : "Run Analysis first.",
            AppTheme::textMuted);
        return;
    }
    for (auto& [sid, topics] : c.relief.perStudent) {
        std::string ts;
        for (int tid : topics) ts += state.graph.get(tid).label + " ";
        ImGui::Text("  %s -> %s", state.graph.get(sid).label.c_str(), ts.c_str());
    }
    ImGui::Spacing();
    coloredText("Topic gain:", AppTheme::textMuted);
    for (auto& [tid, gain] : c.relief.topicGain)
        ImGui::Text("  %s unlocks %d student(s)", state.graph.get(tid).label.c_str(), gain);
}

static void showFlexibilitySection(AppState& state, CanvasState& c) {
    if (c.flexibility.studentLevel.empty()) {
        coloredText("Run Analysis first.", AppTheme::textMuted); return;
    }
    ImGui::BulletText("Flexibility index: %.2f", c.flexibility.flexibilityIndex);
    Tooltips::ifHovered(Tooltips::flexIndex);
    for (auto& [sid, level] : c.flexibility.studentLevel) {
        bool forced = level == FlexibilityLevel::Forced;
        ImGui::TextColored(forced ? AppTheme::textBad : AppTheme::textGood,
            "  %s  %s", state.graph.get(sid).label.c_str(), forced ? "Forced" : "Flexible");
        if (!forced) {
            auto it = c.flexibility.alternativeTopics.find(sid);
            if (it != c.flexibility.alternativeTopics.end()) {
                ImGui::SameLine();
                std::string alts;
                for (int tid : it->second) alts += state.graph.get(tid).label + " ";
                ImGui::TextColored(AppTheme::textMuted, "(alt: %s)", alts.c_str());
            }
        }
    }
}

static void showFairnessSection(AppState& state, RoundHistory& history, const char* tableId) {
    if (history.completedPeriods.empty()) {
        coloredText("No round history yet.", AppTheme::textMuted); return;
    }

    auto students = state.graph.getByType(NodeType::Student);
    const char* pnames[] = {"J1", "J2", "U1", "U2", "S1", "S2"};

    int cols = 2 + (int)history.completedPeriods.size();
    float rowH = ImGui::GetTextLineHeight() + ImGui::GetStyle().CellPadding.y * 2.f;
    float tableH = rowH * (1.f + (float)students.size());
    if (!ImGui::BeginTable(tableId, cols,
        ImGuiTableFlags_Borders | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_ScrollX,
        {0.f, tableH}))
        return;

    ImGui::TableSetupColumn("Student");
    for (auto rnd : history.completedPeriods)
        ImGui::TableSetupColumn(pnames[(int)rnd]);
    ImGui::TableSetupColumn("Missed");
    ImGui::TableHeadersRow();

    for (int sid : students) {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted(state.graph.get(sid).label.c_str());
        int missed = 0, col = 1;
        for (auto rnd : history.completedPeriods) {
            ImGui::TableSetColumnIndex(col++);
            bool matched = false;
            auto it = history.assignments.find(sid);
            if (it != history.assignments.end())
                for (auto& [ep, tid] : it->second)
                    if (ep == rnd) { matched = true; break; }
            if (matched) coloredText("ok", AppTheme::textGood);
            else { coloredText("miss", AppTheme::textBad); ++missed; }
        }
        ImGui::TableSetColumnIndex(col);
        if (missed) ImGui::TextColored(AppTheme::textBad, "%d", missed);
        else coloredText("-", AppTheme::textMuted);
    }
    ImGui::EndTable();
}

void App::rightPanel() {
    auto& tc = m_state.topCanvas;
    auto& bc = m_state.botCanvas;

    auto algoName = [](AlgoChoice a) -> const char* {
        return a == AlgoChoice::Simple ? "Simple" : "Hopcroft-Karp";
    };
    auto algoColor = [](AlgoChoice a) -> ImVec4 {
        return a == AlgoChoice::Simple ? AppTheme::student : AppTheme::topic;
    };
    auto bothCanvases = [&](auto fnTop, auto fnBot) {
        coloredText(algoName(tc.algo), algoColor(tc.algo)); fnTop();
        if (m_state.splitView) { coloredText(algoName(bc.algo), algoColor(bc.algo)); fnBot(); }
    };

    if (ImGui::CollapsingHeader("Matching", ImGuiTreeNodeFlags_DefaultOpen)) {
        Tooltips::ifHovered(Tooltips::matching);
        bothCanvases([&] { showMatchingSection(m_state, tc, "Assignments##s"); },
                     [&] { showMatchingSection(m_state, bc, "Assignments##hk"); });
    }

    if (ImGui::CollapsingHeader("Cover + DCA")) {
        Tooltips::ifHovered(Tooltips::cover);
        bothCanvases([&] { showCoverSection(m_state, tc); },
                     [&] { showCoverSection(m_state, bc); });
    }

    if (ImGui::CollapsingHeader("Relief")) {
        Tooltips::ifHovered(Tooltips::relief);
        showReliefSection(m_state, tc);
    }

    if (ImGui::CollapsingHeader("Flexibility")) {
        Tooltips::ifHovered(Tooltips::flexibility);
        showFlexibilitySection(m_state, tc);
    }

    if (ImGui::CollapsingHeader("Fairness")) {
        Tooltips::ifHovered(Tooltips::fairness);
        if (m_state.splitView) {
            coloredText(algoName(tc.algo), algoColor(tc.algo));
            ImGui::SameLine(); ImGui::TextColored(AppTheme::textMuted, "(left)");
            showFairnessSection(m_state, m_state.topCanvas.history, "##fairS");
            coloredText(algoName(bc.algo), algoColor(bc.algo));
            ImGui::SameLine(); ImGui::TextColored(AppTheme::textMuted, "(right)");
            showFairnessSection(m_state, m_state.botCanvas.history, "##fairHK");
        } else {
            showFairnessSection(m_state, m_state.history, "##fair");
        }
    }
}
