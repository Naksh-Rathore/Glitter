#include <cstring>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stb_image.h>

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

glm::vec3 lightDir = glm::normalize(glm::vec3(50.0f, -75.0f, 100.0f));

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

    Shader shader("Glitter/Assets/shaders");

    shader.use();
    setShaderUniforms(shader);
    glUseProgram(0);

    const unsigned int SHADOW_WIDTH = 4096, SHADOW_HEIGHT = 1024;

    GLuint depthMapFbo;

    glGenFramebuffers(1, &depthMapFbo);

    GLuint depthMap;
    
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    Shader depthMapShader("Glitter/Assets/depth-map-shaders");

    Model model("Glitter/Assets/dining-room/dining-room.obj");

    while (!glfwWindowShouldClose(window)) {    
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFbo);

        glClear(GL_DEPTH_BUFFER_BIT);

        glm::vec3 lightPos = -lightDir * 10.0f;

        glm::mat4 lightView = glm::lookAt(
            lightPos,
            glm::vec3(0.0f),   // scene center
            glm::vec3(0.0f, 1.0f, 0.0f)
        );

        glm::mat4 lightProj = glm::ortho(
            -10.0f, 10.0f,
            -10.0f, 10.0f,
            -10.0f, 20.0f
        );

        glm::mat4 lightSpaceMatrix = lightProj * lightView;

        shader.use();
        shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
        glUseProgram(0);

        depthMapShader.use();
        depthMapShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

        glm::mat4 modelMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -2.0f));
        depthMapShader.setMat4("model", modelMat);

        glCullFace(GL_FRONT);

        model.draw(depthMapShader);

        glCullFace(GL_BACK);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        int fbWidth, fbHeight;

        glfwGetFramebufferSize(window, &fbWidth, &fbHeight);

        glViewport(0, 0, fbWidth, fbHeight);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, depthMap);

        shader.use();

        shader.setVec3("viewPos", camera.pos());

        shader.setMat4("projection", glm::perspective(glm::radians(camera.m_zoom), SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 100.0f));
        shader.setMat4("view", camera.viewMatrix());

        model.draw(shader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return EXIT_SUCCESS;
}


void setShaderUniforms(Shader& shader) {
    shader.setMat4("model", glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -2.0f)));
    shader.setMat4("projection", glm::perspective(glm::radians(camera.m_zoom), SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 100.0f));
    shader.setMat4("view", camera.viewMatrix());
    
    shader.setVec3("light.direction", lightDir); 

    shader.setVec3("light.ambient", glm::vec3(0.22f));
    shader.setVec3("light.diffuse", glm::vec3(0.5f));
    shader.setVec3("light.specular", glm::vec3(1.0f));

    shader.setFloat("light.constant", 1.0f);
    shader.setFloat("light.linear", 0.09f);
    shader.setFloat("light.quadratic", 0.032f);

    shader.setFloat("material.shininess", 128.0f);

    shader.setFloat("ambientStrength", 0.15f);

    shader.setInt("shadowMap", 2);
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

