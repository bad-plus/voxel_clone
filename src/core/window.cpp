#include "window.h"
#include "logger.h"
#include "../render/camera.hpp"
#include "game.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

Window::Window(const char* title, int width, int height, GameContext* context) {
    m_glfw_window = glfwCreateWindow(width, height, title, nullptr, nullptr);

    if(!m_glfw_window) {
        LOG_ERROR("Failed to create GLFW window");
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(m_glfw_window);

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    setContext(context);
    
    bindCallbacks();
}

void Window::setContext(GameContext* context) {
    glfwSetWindowUserPointer(m_glfw_window, context);
}

Window::~Window() {
    
}

bool Window::isClose() {
    return glfwWindowShouldClose(m_glfw_window);
}

void Window::setCursorEnabled(bool status) {
    if(!status) glfwSetInputMode(m_glfw_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    else glfwSetInputMode(m_glfw_window, GLFW_CURSOR, GLFW_CURSOR);
}

void Window::quit() {
    glfwSetWindowShouldClose(m_glfw_window, GL_TRUE);
}

GLFWwindow* Window::getGLFWwindow() {
    return m_glfw_window;
}