#include "window.h"
#include "../logger.h"
#include "../../render/camera.hpp"
#include "../game.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
    
Window::Window(const char* title, int width, int height, Input* input) {
    m_glfw_window = glfwCreateWindow(width, height, title, nullptr, nullptr);

    if(!m_glfw_window) {
        throw std::runtime_error("Failed to create GLFW window");
    }

    glfwMakeContextCurrent(m_glfw_window);

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    glfwSetWindowUserPointer(m_glfw_window, input);
    glfwSetWindowAttrib(m_glfw_window, GLFW_RESIZABLE, GLFW_TRUE);
    
    bindCallbacks();
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

void Window::setWindowSize(const int width, const int height) {
    glfwSetWindowSize(m_glfw_window, width, height);
}