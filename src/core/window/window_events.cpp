#include "window.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

void Window::eventProcessing() {
    glfwPollEvents();
}