#ifndef SEMINARALLOCATIONENGINE_APPTOOLTIPS_H
#define SEMINARALLOCATIONENGINE_APPTOOLTIPS_H

#include "imgui.h"

namespace Tooltips {

inline void hint(const char* label, const char* text) {
    ImGui::SameLine();
    ImGui::TextDisabled("%s", label);
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(300.f);
        ImGui::TextUnformatted(text);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

inline void ifHovered(const char* text) {
    if (!ImGui::IsItemHovered()) return;
    ImGui::BeginTooltip();
    ImGui::PushTextWrapPos(300.f);
    ImGui::TextUnformatted(text);
    ImGui::PopTextWrapPos();
    ImGui::EndTooltip();
}

constexpr auto matching =
    "Maximum bipartite matching: pairs each student with at most one topic and "
    "each topic with at most one student, maximising the number of matched pairs. "
    "Solved here by both Simple BFS and Hopcroft-Karp.";

constexpr auto simpleAlgo =
    "Augmenting-path matching: each BFS call finds one shortest augmenting path "
    "and flips it. Runs at most |V| phases, each O(E). Total: O(V * E).";

constexpr auto hkAlgo =
    "Hopcroft-Karp: one BFS builds a layered (level) graph from all free left "
    "vertices simultaneously, then DFS extracts a maximal set of vertex-disjoint "
    "shortest augmenting paths in one phase. O(sqrt(V) * E) total - "
    "significantly faster than simple augmentation on dense graphs.";

constexpr auto cover =
    "Minimum vertex cover (Konig's theorem): the smallest set of nodes that "
    "touches every preference edge. In bipartite graphs its size always equals "
    "the maximum matching size. Built from the matching via an alternating BFS.";

constexpr auto dcaConfirmed =
    "Confirmed bottleneck: topic demand exceeds average AND it appears in the "
    "cover. The matching could not route all competing students elsewhere - "
    "this topic is a genuine capacity constraint.";

constexpr auto dcaResolved =
    "Resolved: topic demand exceeds average but it does NOT appear in the cover. "
    "Despite the competition, the matching found enough augmenting paths around "
    "it - demand pressure was absorbed by alternative routes.";

constexpr auto dcaStructural =
    "Structural cover node: topic demand is below average yet it still appears "
    "in the cover. This is an artifact of the alternating-path construction "
    "(swap chains), not a real capacity bottleneck.";

constexpr auto dcaIndex =
    "Absorption rate = Resolved / (Confirmed + Resolved). "
    "Measures how well the matching handled high-demand topics. "
    "1.0 means every contested topic was routed around; 0.0 means every one became a bottleneck.";

constexpr auto relief =
    "Relief advisor: for each unmatched student, lists topics outside their "
    "preference list whose addition would open an augmenting path. "
    "Shows where expanding topic capacity or student lists has the most impact.";

constexpr auto flexibility =
    "Assignment necessity: checks whether each matched pair appears in every "
    "maximum matching (Forced) or only in some (Flexible). "
    "A forced assignment cannot be replaced without reducing matching size.";

constexpr auto flexIndex =
    "Flexibility index = Forced pairs / Total matched pairs. "
    "1.0 means the matching is unique - no assignment can change. "
    "0.0 means every pair has an equivalent substitute in another max matching.";

constexpr auto fairness =
    "Cross-round fairness table: shows for each student whether they were "
    "matched (ok) or unmatched (miss) in each committed exam period. "
    "The Missed column counts total unmatched rounds per student.";

constexpr auto viewFlow =
    "Network flow view: adds a super-source S connected to every student "
    "(capacity 1) and a super-sink T connected from every topic (capacity 1). "
    "Edge labels show flow value (0 or 1). Max-flow equals max matching size "
    "by the standard bipartite-to-flow reduction.";

constexpr auto blockedEdge =
    "Forbidden preference: this student was already assigned this topic in an "
    "earlier exam period. The edge is excluded from the current round's matching "
    "to enforce the no-repeat rule across periods.";

constexpr auto rankCtrl =
    "Priority rank for the next preference edge you draw. "
    "Rank 1 = first choice; higher numbers = lower priority. "
    "Ranks are shown on edges and used to report which preference each student received.";

constexpr auto maxPrefs =
    "Upper bound on preference edges per student. "
    "Reflects a real seminar constraint: students submit a ranked shortlist, "
    "not an unconstrained wish list. Default is 3.";

constexpr auto analysis =
    "Computes Relief Advisor and Matching Flexibility for the top canvas (requires matching).";

constexpr auto commitRound =
    "Save this matching to round history and advance to the next period.";

constexpr auto animSpeed =
    "Animation step delay (seconds per phase).";

} // namespace Tooltips

#endif
