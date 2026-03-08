#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

constexpr float SCREEN_WIDTH = 1920.0f;
constexpr float SCREEN_HEIGHT = 1080.0f;

namespace Init {
    void framebuffer_size_callback(GLFWwindow *window, int width, int height);

    void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, 
                            GLsizei length, const char *message, const void *userParam);

    GLFWwindow* init();
};

