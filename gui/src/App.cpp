#include "App.h"
#include "AppTheme.h"
#include "GraphRenderer.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

App::App() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(1400, 860, "Seminar Allocation Engine", nullptr, nullptr);
    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    AppTheme::apply();

    m_lastTime = glfwGetTime();
}

App::~App() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void App::run() {
    while (!glfwWindowShouldClose(m_window)) {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        renderFrame();

        ImGui::Render();
        int fw, fh;
        glfwGetFramebufferSize(m_window, &fw, &fh);
        glViewport(0, 0, fw, fh);
        glClearColor(AppTheme::windowBg.x, AppTheme::windowBg.y, AppTheme::windowBg.z, AppTheme::windowBg.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(m_window);
    }
}

void App::renderFrame() {
    double now = glfwGetTime();
    float dt = (float)(now - m_lastTime);
    m_lastTime = now;
    m_state.tickAnimation(dt);

    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowPos({0, 0});
    ImGui::SetNextWindowSize(io.DisplaySize);
    ImGui::Begin("##root", nullptr,
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoScrollbar);

    const float leftW = 215.f, rightW = 275.f, gap = 4.f;
    float totalH = ImGui::GetContentRegionAvail().y;
    float centerW = ImGui::GetContentRegionAvail().x - leftW - rightW - gap * 2.f;

    auto panel = [&](const char* id, float w, auto fn) {
        ImGui::PushStyleColor(ImGuiCol_ChildBg, AppTheme::panelBg);
        ImGui::BeginChild(id, {w, totalH}, true);
        ImGui::PopStyleColor();
        fn();
        ImGui::EndChild();
    };

    panel("##left", leftW, [&] { leftPanel(); }); ImGui::SameLine(0, gap);

    ImGui::BeginChild("##center", {centerW, totalH}, false);
    ImVec2 org = ImGui::GetCursorScreenPos();
    ImVec2 sz = ImGui::GetContentRegionAvail();
    if (m_state.splitView) {
        float half = (sz.x - gap) * 0.5f;
        renderCanvas(m_state, m_state.topCanvas, org, {half, sz.y});
        renderCanvas(m_state, m_state.botCanvas, {org.x + half + gap, org.y}, {half, sz.y});
    } else {
        renderCanvas(m_state, m_state.topCanvas, org, sz);
    }
    ImGui::SetCursorScreenPos(org);
    ImGui::InvisibleButton("##canvas_bg", sz);
    ImGui::EndChild(); ImGui::SameLine(0, gap);

    panel("##right", rightW, [&] { rightPanel(); });

    ImGui::End();
}
