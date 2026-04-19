#ifndef SEMINARALLOCATIONENGINE_APPTHEME_H
#define SEMINARALLOCATIONENGINE_APPTHEME_H
#include "imgui.h"

namespace AppTheme {
    static constexpr ImVec4 student = {0.20f, 0.47f, 0.90f, 1.f};
    static constexpr ImVec4 studentDim = {0.20f, 0.47f, 0.90f, 0.4f};
    static constexpr ImVec4 studentInactive = {0.7f, 0.7f, 0.7f, 0.5f};
    static constexpr ImVec4 topic = {0.93f, 0.55f, 0.13f, 1.f};
    static constexpr ImVec4 topicDim = {0.93f, 0.55f, 0.13f, 0.4f};
    static constexpr ImVec4 nodeText = {1.f, 1.f, 1.f, 1.f};
    static constexpr float nodeRadius = 18.f;

    static constexpr ImVec4 edgePref = {0.60f, 0.60f, 0.60f, 1.f};
    static constexpr ImVec4 edgeBlocked = {0.65f, 0.65f, 0.65f, 0.3f};
    static constexpr ImVec4 edgeMatched = {0.20f, 0.47f, 0.90f, 1.f};
    static constexpr ImVec4 edgeMatchedHK = {0.62f, 0.20f, 0.90f, 1.f};
    static constexpr float edgePrefThick = 1.5f;
    static constexpr float edgeMatchThick = 3.0f;

    static constexpr ImVec4 coverConfirmed  = {0.85f, 0.15f, 0.15f, 1.f};
    static constexpr ImVec4 coverStructural = {0.93f, 0.55f, 0.13f, 1.f};
    static constexpr ImVec4 coverResolved   = {0.20f, 0.72f, 0.30f, 1.f};
    static constexpr ImVec4 coverRing       = {0.86f, 0.20f, 0.20f, 0.86f};
    static constexpr float coverBorderW = 4.0f;

    static constexpr ImVec4 animPath    = {0.05f, 0.82f, 0.95f, 1.f};
    static constexpr ImVec4 animDimPref = {0.60f, 0.60f, 0.60f, 0.15f};
    static constexpr ImVec4 animDimMatch= {0.55f, 0.55f, 0.55f, 0.25f};
    static constexpr float animThick = 5.5f;

    static constexpr ImVec4 canvasBg = {0.96f, 0.96f, 0.96f, 1.f};
    static constexpr ImVec4 flowSrcSink = {0.30f, 0.30f, 0.30f, 1.f};
    static constexpr ImVec4 windowBg = {0.88f, 0.88f, 0.88f, 1.f};

    static constexpr ImU32 canvasBorder = IM_COL32(180, 180, 180, 255);
    static constexpr ImU32 canvasLabel = IM_COL32(150, 150, 150, 255);
    static constexpr ImU32 panelBg = IM_COL32(235, 235, 238, 255);
    static constexpr ImU32 rankLabel = IM_COL32(110, 110, 110, 200);
    static constexpr ImU32 inactiveStrike = IM_COL32(80, 80, 80, 180);
    static constexpr ImU32 hoverRing = IM_COL32(50, 50, 50, 160);
    static constexpr ImU32 flowMatched = IM_COL32(100, 180, 100, 180);
    static constexpr ImU32 flowUnmatched = IM_COL32(180, 180, 180, 100);
    static constexpr ImU32 flowLabel = IM_COL32(80, 80, 80, 200);
    static constexpr ImU32 white = IM_COL32(255, 255, 255, 255);

    static constexpr ImVec4 btnPrimary = {0.20f, 0.47f, 0.90f, 1.f};
    static constexpr ImVec4 btnPrimaryHover = {0.28f, 0.55f, 0.98f, 1.f};
    static constexpr ImVec4 btnPrimaryActive = {0.14f, 0.38f, 0.78f, 1.f};
    static constexpr ImVec4 btnRed = {0.75f, 0.15f, 0.15f, 1.f};
    static constexpr ImVec4 btnRedHover = {0.90f, 0.22f, 0.22f, 1.f};
    static constexpr ImVec4 btnRedActive = {0.60f, 0.08f, 0.08f, 1.f};
    static constexpr ImVec4 textMuted = {0.55f, 0.55f, 0.55f, 1.f};
    static constexpr ImVec4 textGood = {0.18f, 0.65f, 0.28f, 1.f};
    static constexpr ImVec4 textWarn = {0.85f, 0.50f, 0.08f, 1.f};
    static constexpr ImVec4 textBad = {0.75f, 0.12f, 0.12f, 1.f};

    inline ImU32 col(ImVec4 c) { return ImGui::ColorConvertFloat4ToU32(c); }

    inline void apply() {
        ImGui::StyleColorsLight();
        ImGuiStyle& s = ImGui::GetStyle();
        s.WindowRounding = 6.f; s.FrameRounding = 4.f; s.GrabRounding = 4.f;
        s.ItemSpacing = {8.f, 6.f}; s.FramePadding = {6.f, 4.f};
    }
}

#endif
