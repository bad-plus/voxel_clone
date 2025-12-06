#pragma once

struct GLFWwindow;
struct Camera;
struct GameContext;

class Window {
public:
    Window(const char* title, int width, int height, GameContext* context);
    ~Window();

    bool isClose();
    void eventProcessing();
    void bindCallbacks();
    void setCursorEnabled(bool status);

    void setContext(GameContext* context);

    void quit();

    GLFWwindow* getGLFWwindow();
private:
    GLFWwindow* m_glfw_window;
};