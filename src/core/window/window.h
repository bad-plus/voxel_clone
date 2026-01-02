#pragma once

struct GLFWwindow;
struct Camera;
struct Input;

class Window {
public:
    Window(const char* title, int width, int height, Input* input);
    ~Window() = default;

    bool isClose();
    void eventProcessing();
    void bindCallbacks();
    void setCursorEnabled(bool status);
    void setWindowSize(const int width, const int height);

    void quit();

    GLFWwindow* getGLFWwindow();
private:
    GLFWwindow* m_glfw_window;
};