#include <cstring>
#include <stdexcept>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stb_image.h>
#include <stdexcept>

#include "camera.h"
#include "shader.h"
#include "model.h"
#include "init.h"

FreeCamera camera(glm::vec3(0.0f), 5.0f, 0.125f, 45.0f);

float deltaTime;
float lastFrame;

void mouseCallback([[maybe_unused]] GLFWwindow* window, double xposIn, double yposIn);
void scrollCallback([[maybe_unused]] GLFWwindow* window, [[maybe_unused]] double xoffset, double yoffset);

void processInput(GLFWwindow *window);

void setShaderUniforms(Shader& shader);

unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad();

float exposure = 1.5f;

int main(int argc, char **argv) {

    bool shouldFullscreen = false;
    bool useDebugContext = true;

    for (int i = 1; i < argc; i++) {
        if (std::strcmp(argv[i], "--fullscreen") == 0)
            shouldFullscreen = true;
        else if (std::strcmp(argv[i], "--no-debug") == 0) 
            useDebugContext = false;
    }

    GLFWwindow *window = Init::init(shouldFullscreen, useDebugContext);

    if (window == nullptr) 
        return EXIT_FAILURE;

    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_FRAMEBUFFER_SRGB);

    Model model("Glitter/Assets/dining-room/dining-room.obj");

    Shader shader("Glitter/Assets/shaders");

    shader.use();
    setShaderUniforms(shader);
    glUseProgram(0);

    GLuint hdrFbo;
    glGenFramebuffers(1, &hdrFbo);

    GLuint colorBuffer;

    glGenTextures(1, &colorBuffer);
    glBindTexture(GL_TEXTURE_2D, colorBuffer);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLuint rboDepth;

    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);

    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCREEN_WIDTH, SCREEN_HEIGHT);

    glBindFramebuffer(GL_FRAMEBUFFER, hdrFbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::runtime_error("Framebuffer not complete!");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    Shader hdrShader("Glitter/Assets/hdr-shader");

    hdrShader.use();
    hdrShader.setInt("hdrBuffer", 0);
    glUseProgram(0);

    while (!glfwWindowShouldClose(window)) {    
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindFramebuffer(GL_FRAMEBUFFER, hdrFbo);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();
        
        shader.setMat4("projection", glm::perspective(glm::radians(camera.m_zoom), (float) SCREEN_WIDTH / (float) SCREEN_HEIGHT, 0.1f, 100.0f));
        shader.setMat4("view", camera.viewMatrix());

        shader.setVec3("viewPos", camera.pos());

        model.draw(shader);        

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        int fbWidth, fbHeight;
        glfwGetFramebufferSize(window, &fbWidth, &fbHeight);

        glViewport(0, 0, fbWidth, fbHeight);

        hdrShader.use();

        hdrShader.setFloat("exposure", exposure);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, colorBuffer);

        renderQuad();

        glUseProgram(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return EXIT_SUCCESS;
}

void setShaderUniforms(Shader& shader) {
    shader.setMat4("model", glm::translate(glm::mat4(1.0f), glm::vec3(0.0f)));
    shader.setMat4("projection", glm::perspective(glm::radians(camera.m_zoom), SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 100.0f));
    shader.setMat4("view", camera.viewMatrix());

    shader.setVec3("light.position", glm::vec3(0.0f));

    shader.setVec3("light.ambient", glm::vec3(0.22f));
    shader.setVec3("light.diffuse", glm::vec3(0.5f));
    shader.setVec3("light.specular", glm::vec3(1.0f));

    shader.setFloat("light.constant", 1.0f);
    shader.setFloat("light.linear", 0.09f);
    shader.setFloat("light.quadratic", 0.032f);

    shader.setFloat("material.shininess", 128.0f);
}

void mouseCallback([[maybe_unused]] GLFWwindow* window, double xposIn, double yposIn) {
    camera.processMouseInput(xposIn, yposIn);
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.processKeyboardInput(CameraDirection::FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.processKeyboardInput(CameraDirection::BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.processKeyboardInput(CameraDirection::LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.processKeyboardInput(CameraDirection::RIGHT, deltaTime);
}

void scrollCallback([[maybe_unused]] GLFWwindow* window, [[maybe_unused]] double xoffset, double yoffset) {
    camera.processScrollInput(yoffset);
}

void renderQuad() {
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

