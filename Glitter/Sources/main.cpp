#include <cstdlib>
#include <cstring>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stb_image.h>

#include "camera.h"
#include "shader.h"
#include "model.h"
#include "init.h"
#include "mesh.h"
#include "texture.h"
#include "common_vertices.h"

FreeCamera camera(glm::vec3(0.0f), 5.0f, 0.125f, 45.0f);

float deltaTime;
float lastFrame;

void mouseCallback([[maybe_unused]] GLFWwindow* window, double xposIn, double yposIn);
void scrollCallback([[maybe_unused]] GLFWwindow* window, [[maybe_unused]] double xoffset, double yoffset);

void processInput(GLFWwindow *window);

int main(int argc, char **argv) {

    bool shouldFullscreen = false;
    bool shouldAttenuate = false;
    bool useDebugContext = true;

    for (int i = 1; i < argc; i++) {
        if (std::strcmp(argv[i], "--fullscreen") == 0)
            shouldFullscreen = true;
        else if (std::strcmp(argv[i], "--attenuate") == 0)
            shouldAttenuate = true;
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

    Model diningRoomModel("Glitter/Assets/dining-room/dining-room.obj");
    Model backpackModel("Glitter/Assets/backpack/backpack.obj", true);

    std::vector<BasicTexture> boxTextures {
        { "texture_diffuse", loadTextureFromFile("container2.png", "Glitter/Assets/box"), "container2.png" },
        { "texture_specular", loadTextureFromFile("container2_specular.png", "Glitter/Assets/box", false), "container2_specular.png" },
    };

    Mesh boxMesh(CommonVertices::CubeVertices, CommonVertices::CubeIndices, boxTextures);

    stbi_set_flip_vertically_on_load(false);
    GLuint skyboxTexture = loadCubeMapFromDirectory("Glitter/Assets/skybox", ".jpg");
    stbi_set_flip_vertically_on_load(true);

    Shader skyboxShader("Glitter/Assets/skybox");

    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);
    glUseProgram(0);

    float skyboxVertices[] = {
        -1.0f,  1.0f, -1.0f,  -1.0f, -1.0f, -1.0f,   1.0f, -1.0f, -1.0f,   1.0f, -1.0f, -1.0f,   1.0f,  1.0f, -1.0f,  -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,  -1.0f, -1.0f, -1.0f,  -1.0f,  1.0f, -1.0f,  -1.0f,  1.0f, -1.0f,  -1.0f,  1.0f,  1.0f,  -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,   1.0f, -1.0f,  1.0f,   1.0f,  1.0f,  1.0f,   1.0f,  1.0f,  1.0f,   1.0f,  1.0f, -1.0f,   1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,  -1.0f,  1.0f,  1.0f,   1.0f,  1.0f,  1.0f,   1.0f,  1.0f,  1.0f,   1.0f, -1.0f,  1.0f,  -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,   1.0f,  1.0f, -1.0f,   1.0f,  1.0f,  1.0f,   1.0f,  1.0f,  1.0f,  -1.0f,  1.0f,  1.0f,  -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,  -1.0f, -1.0f,  1.0f,   1.0f, -1.0f, -1.0f,   1.0f, -1.0f, -1.0f,  -1.0f, -1.0f,  1.0f,   1.0f, -1.0f,  1.0f
    };

    GLuint skyboxVAO, skyboxVBO, skyboxEBO;

    skyboxEBO = 0;

    uploadMeshBuffers(skyboxVertices, sizeof(skyboxVertices), skyboxVAO, skyboxVBO, skyboxEBO);

    Shader shader("Glitter/Assets/shaders");

    shader.use();

    shader.setMat4("model", glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -2.0f)));
    shader.setMat4("projection", glm::perspective(glm::radians(camera.m_zoom), SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 100.0f));
    shader.setMat4("view", camera.viewMatrix());

    shader.setVec3("light.position", glm::vec3(0.0f, 1.0f, 0.0f));

    shader.setVec3("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
    shader.setVec3("light.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
    shader.setVec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));

    shader.setFloat("light.constant", 1.0f);
    shader.setFloat("light.linear", 0.09f);
    shader.setFloat("light.quadratic", 0.032f);

    shader.setFloat("material.shininess", 128.0f);

    shader.setBool("shouldAttenuate", shouldAttenuate);

    while (!glfwWindowShouldClose(window)) {    
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();
        
        shader.setMat4("projection", glm::perspective(glm::radians(camera.m_zoom), (float) SCREEN_WIDTH / (float) SCREEN_HEIGHT, 0.1f, 100.0f));
        shader.setMat4("view", camera.viewMatrix());

        shader.setVec3("viewPos", camera.pos());
        
        boxMesh.draw(shader);

        //diningRoomModel.draw(shader);
        //backpackModel.draw(shader);

        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
                                 
        skyboxShader.use();

        glm::mat4 view = glm::mat4(glm::mat3(camera.viewMatrix())); // remove translation from the view matrix
                                                                    
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", glm::perspective(glm::radians(camera.m_zoom), SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 100.0f));

        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glBindVertexArray(0);
        glDepthFunc(GL_LESS); 

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

void scrollCallback([[maybe_unused]] GLFWwindow* window, [[maybe_unused]] double xoffset, double yoffset) {
    camera.processScrollInput(yoffset);
}

