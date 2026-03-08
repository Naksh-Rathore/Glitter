#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Not constexpr since it is determined by a flag
inline float SCREEN_WIDTH = 800.0f;
inline float SCREEN_HEIGHT = 600.0f;

namespace Init {
    void framebuffer_size_callback(GLFWwindow *window, int width, int height);

    void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, 
                            GLsizei length, const char *message, const void *userParam);

    GLFWwindow* init(bool shouldFullscreen);
};

