#include <cstdlib>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stb_image.h>

#include <iostream>

#include "camera.h"
#include "shader.h"
#include "model.h"
#include "init.h"

FreeCamera camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 15.0f, 0.125f);

float deltaTime;
float lastFrame;

void mouseCallback([[maybe_unused]] GLFWwindow* window, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (camera.firstMouse()) {
        camera.setLastX(xpos);
        camera.setLastY(ypos);
            
        camera.setFirstMouse(false);
        return;
    }

    float xoffset = xpos - camera.lastX();
    float yoffset = camera.lastY() - ypos; // reversed since y-coordinates go from bottom to top

    camera.setLastX(xpos);
    camera.setLastY(ypos);

    camera.processMouseInput(xoffset, yoffset);
}


int main() {

    GLFWwindow *window = Init::init();

    if (window == nullptr) 
        return EXIT_FAILURE;

    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glEnable(GL_DEPTH_TEST);

  Shader shader("Glitter/Assets");

  shader.use();

  shader.setMat4("model", glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -2.0f)));
  shader.setMat4("projection", glm::perspective(glm::radians(45.0f), 800.0f / 800.0f, 0.1f, 100.0f));
  shader.setMat4("view", camera.viewMatrix());

  std::cout << "Starting model load" << "\n";

  Model model("Glitter/Assets/backpack/backpack.obj");

  std::cout << "Finished model load" << "\n";


  // Rendering Loop
  while (!glfwWindowShouldClose(window)) {
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

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

    // Background Fill Color
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    shader.use();

    shader.setMat4("view", camera.viewMatrix());

    shader.setFloat("material.shininess", 32.0f);

    shader.setVec3("light.position", glm::vec3(10.0f));
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
