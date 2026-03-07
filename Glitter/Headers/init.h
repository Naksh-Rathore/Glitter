#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800

namespace Init {
    void framebuffer_size_callback(GLFWwindow *window, int width, int height);

    void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, 
                            GLsizei length, const char *message, const void *userParam);

    GLFWwindow* init();
};

