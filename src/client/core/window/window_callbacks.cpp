#include "window.h"
#include "../../render/camera.hpp"
#include <core/logger.hpp>
#include "../input/input.h"
#include "../game.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

void key_callback(GLFWwindow* window, int key, int scan_code, int action, int mode) {
    Input* input = (Input*)glfwGetWindowUserPointer(window);
    if(action == GLFW_PRESS) input->press_button(key);
    if(action == GLFW_RELEASE) input->release_button(key);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    Input* input = (Input*)glfwGetWindowUserPointer(window);
    input->mouse_move(xpos, ypos);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mode) {
    Input* input = (Input*)glfwGetWindowUserPointer(window);
    if(action == GLFW_PRESS) input->mouse_press_button(button);
    if(action == GLFW_RELEASE) input->mouse_release_button(button);
}

void scroll_callback(GLFWwindow* window, double offset_x, double offset_y) {
    Input* input = (Input*)glfwGetWindowUserPointer(window);
    input->scroll(offset_x, offset_y);
}

void window_size_callback(GLFWwindow* window, int width, int height) {
	Input* input = (Input*)glfwGetWindowUserPointer(window);
    input->update_window_size(width, height);
}

void Window::bindCallbacks() {
    glfwSetKeyCallback(m_glfw_window, key_callback);
    glfwSetScrollCallback(m_glfw_window, scroll_callback);
    glfwSetCursorPosCallback(m_glfw_window, mouse_callback);
    glfwSetMouseButtonCallback(m_glfw_window, mouse_button_callback);
    glfwSetWindowSizeCallback(m_glfw_window, window_size_callback);

    LOG_INFO("Callbacks installed");
}