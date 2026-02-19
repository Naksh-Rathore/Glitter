#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <vector>

#include "mesh.h"
#include "texture.h"
#include "shader.h"
#include "camera.h"

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

  // Load GLFW and Create a Window
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  GLFWwindow* window = glfwCreateWindow(800, 800, "OpenGL", nullptr, nullptr);

  glfwSetCursorPosCallback(window, mouseCallback);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  // Check for Valid Context
  if (window == nullptr) {
    fprintf(stderr, "Failed to Create OpenGL Context");
    return EXIT_FAILURE;
  }

  // Create Context and Load OpenGL Functions
  glfwMakeContextCurrent(window);
  gladLoadGL();
  std::cout << "OpenGL " << glGetString(GL_VERSION) << "\n";

  glViewport(0, 0, 800, 800);
    
  
    std::vector<float> CubeVertices = {
        // -------- Front (+Z) --------
        -0.5f, -0.5f,  0.5f,   0.f, 0.f, 1.f,   0.f, 0.f,
        0.5f, -0.5f,  0.5f,   0.f, 0.f, 1.f,   1.f, 0.f,
        0.5f,  0.5f,  0.5f,   0.f, 0.f, 1.f,   1.f, 1.f,
        -0.5f,  0.5f,  0.5f,   0.f, 0.f, 1.f,   0.f, 1.f,

        // -------- Back (-Z) --------
        0.5f, -0.5f, -0.5f,   0.f, 0.f,-1.f,   0.f, 0.f,
        -0.5f, -0.5f, -0.5f,   0.f, 0.f,-1.f,   1.f, 0.f,
        -0.5f,  0.5f, -0.5f,   0.f, 0.f,-1.f,   1.f, 1.f,
        0.5f,  0.5f, -0.5f,   0.f, 0.f,-1.f,   0.f, 1.f,

        // -------- Left (-X) --------
        -0.5f, -0.5f, -0.5f,  -1.f, 0.f, 0.f,   0.f, 0.f,
        -0.5f, -0.5f,  0.5f,  -1.f, 0.f, 0.f,   1.f, 0.f,
        -0.5f,  0.5f,  0.5f,  -1.f, 0.f, 0.f,   1.f, 1.f,
        -0.5f,  0.5f, -0.5f,  -1.f, 0.f, 0.f,   0.f, 1.f,

        // -------- Right (+X) --------
        0.5f, -0.5f,  0.5f,   1.f, 0.f, 0.f,   0.f, 0.f,
        0.5f, -0.5f, -0.5f,   1.f, 0.f, 0.f,   1.f, 0.f,
        0.5f,  0.5f, -0.5f,   1.f, 0.f, 0.f,   1.f, 1.f,
        0.5f,  0.5f,  0.5f,   1.f, 0.f, 0.f,   0.f, 1.f,

        // -------- Top (+Y) --------
        -0.5f,  0.5f,  0.5f,   0.f, 1.f, 0.f,   0.f, 0.f,
        0.5f,  0.5f,  0.5f,   0.f, 1.f, 0.f,   1.f, 0.f,
        0.5f,  0.5f, -0.5f,   0.f, 1.f, 0.f,   1.f, 1.f,
        -0.5f,  0.5f, -0.5f,   0.f, 1.f, 0.f,   0.f, 1.f,

        // -------- Bottom (-Y) --------
        -0.5f, -0.5f, -0.5f,   0.f,-1.f, 0.f,   0.f, 0.f,
        0.5f, -0.5f, -0.5f,   0.f,-1.f, 0.f,   1.f, 0.f,
        0.5f, -0.5f,  0.5f,   0.f,-1.f, 0.f,   1.f, 1.f,
        -0.5f, -0.5f,  0.5f,   0.f,-1.f, 0.f,   0.f, 1.f
    };

    std::vector<unsigned int> CubeIndices = {
        0,  1,  2,  2,  3,  0,   // front
        4,  5,  6,  6,  7,  4,   // back
        8,  9, 10, 10, 11,  8,   // left
       12, 13, 14, 14, 15, 12,   // right
       16, 17, 18, 18, 19, 16,   // top
       20, 21, 22, 22, 23, 20    // bottom
    };

    std::vector<Texture> Textures = {
        Texture("Glitter/Assets/texture.png", "texture_diffuse"),
        Texture("Glitter/Assets/texture2.png", "texture_specular")
    };

  Mesh mesh(CubeVertices, CubeIndices, Textures);

  mesh.uploadMesh();

  Shader shader("Glitter/Assets");

  shader.use();
  
  shader.setMat4("model", glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -2.0f)));
  shader.setMat4("projection", glm::perspective(glm::radians(45.0f), 800.0f / 800.0f, 0.1f, 100.0f));
  shader.setMat4("view", camera.viewMatrix());

  glEnable(GL_DEPTH_TEST);


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

    shader.setVec3("light.position", glm::vec3(3.0f, 4.0f, 5.0f));
    shader.setVec3("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
    shader.setVec3("light.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
    shader.setVec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));

    shader.setVec3("viewPos", camera.pos());

    mesh.draw(shader);

    // Flip Buffers and Draw
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return EXIT_SUCCESS;
}
