#include "window.h"
#include "../render/camera.hpp"
#include "logger.h"
#include "input.h"
#include "game.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    GameContext* context = (GameContext*)glfwGetWindowUserPointer(window);
    if(action == GLFW_PRESS) context->input->press_button(key);
    if(action == GLFW_RELEASE) context->input->release_button(key);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    GameContext* context = (GameContext*)glfwGetWindowUserPointer(window);
    context->input->mouse_move(xpos, ypos);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mode) {
    GameContext* context = (GameContext*)glfwGetWindowUserPointer(window);
    if(action == GLFW_PRESS) context->input->mouse_press_button(button);
    if(action == GLFW_RELEASE) context->input->mouse_release_button(button);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    //p_camera->processMouseScroll(yoffset);
}

void Window::bindCallbacks() {
    glfwSetKeyCallback(m_glfw_window, key_callback);
    glfwSetScrollCallback(m_glfw_window, scroll_callback);
    glfwSetCursorPosCallback(m_glfw_window, mouse_callback);
    glfwSetMouseButtonCallback(m_glfw_window, mouse_button_callback);


    LOG_INFO("Callbacks installed");
}