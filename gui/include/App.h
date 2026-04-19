#ifndef SEMINARALLOCATIONENGINE_APP_H
#define SEMINARALLOCATIONENGINE_APP_H

#include <GLFW/glfw3.h>
#include "AppState.h"

class App {
private:
    GLFWwindow* m_window = nullptr;
    AppState m_state;
    double m_lastTime = 0.0;
    void renderFrame();
    void leftPanel();
    void rightPanel();
public:
    App();
    ~App();
    void run();
};

#endif
