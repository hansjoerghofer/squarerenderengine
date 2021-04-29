#include "Application/GL.h"

#include <GLFW/glfw3.h>

GLApplicationScope::GLApplicationScope()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

GLApplicationScope::~GLApplicationScope()
{
    glfwTerminate();
}