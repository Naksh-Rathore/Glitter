#include <cstdlib>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

#include <stb_image.h>

#include "camera.h"
#include "shader.h"
#include "model.h"
#include "init.h"

FreeCamera camera(glm::vec3(0.0f), 5.0f, 0.125f, 45.0f);

float deltaTime;
float lastFrame;

void mouseCallback([[maybe_unused]] GLFWwindow* window, double xposIn, double yposIn);
void scrollCallback([[maybe_unused]] GLFWwindow* window, double xoffset, double yoffset);

void processInput(GLFWwindow *window);

int main() {

    GLFWwindow *window = Init::init();

    if (window == nullptr) 
        return EXIT_FAILURE;

    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glEnable(GL_DEPTH_TEST);

    Shader shader("Glitter/Assets");

    shader.use();

    shader.setMat4("model", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -2.0f)), glm::vec3(0.25f)));
    shader.setMat4("projection", glm::perspective(glm::radians(camera.m_zoom), (float) SCREEN_WIDTH / (float) SCREEN_HEIGHT, 0.1f, 100.0f));
    shader.setMat4("view", camera.viewMatrix());

    Model model("Glitter/Assets/backpack/backpack.obj", true);

    // Rendering Loop
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        // Background Fill Color
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        std::cout << camera.m_zoom << "\n";

        shader.use();
        
        shader.setMat4("projection", glm::perspective(glm::radians(camera.m_zoom), (float) SCREEN_WIDTH / (float) SCREEN_HEIGHT, 0.1f, 100.0f));
        shader.setMat4("view", camera.viewMatrix());

        shader.setFloat("material.shininess", 32.0f);

        shader.setVec3("light.position", glm::vec3(100.0f));
        shader.setVec3("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
        shader.setVec3("light.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
        shader.setVec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));

        model.draw(shader);

        // Flip Buffers and Draw
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return EXIT_SUCCESS;
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

void scrollCallback([[maybe_unused]] GLFWwindow* window, double xoffset, double yoffset) {
    camera.processScrollInput(yoffset);
}

