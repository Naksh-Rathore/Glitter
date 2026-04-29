#include <cstring>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stb_image.h>

#include "camera.h"
#include "shader.h"
#include "init.h"
#include "texture.h"

FreeCamera camera(glm::vec3(0.0f), 5.0f, 0.125f, 45.0f);

float deltaTime;
float lastFrame;

void mouseCallback([[maybe_unused]] GLFWwindow* window, double xposIn, double yposIn);
void scrollCallback([[maybe_unused]] GLFWwindow* window, [[maybe_unused]] double xoffset, double yoffset);

void processInput(GLFWwindow *window);

void setShaderUniforms(Shader& shader);

glm::vec3 lightPos = glm::vec3(0.0f, 0.0f, 0.25f);

bool normalMappingEnabled = false;
bool normalMappingKeyPressed = false;

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

    // Usually model loaders (Assimp, tinyGLTF) compute tangent & bitangent for us

    glm::vec3 pos1(-1.0,  1.0, 0.0);
    glm::vec3 pos2(-1.0, -1.0, 0.0);
    glm::vec3 pos3( 1.0, -1.0, 0.0);
    glm::vec3 pos4( 1.0,  1.0, 0.0);

    glm::vec2 uv1(0.0, 1.0);
    glm::vec2 uv2(0.0, 0.0);
    glm::vec2 uv3(1.0, 0.0);
    glm::vec2 uv4(1.0, 1.0);

    glm::vec3 nm(0.0, 0.0, 1.0);  

    glm::vec3 edge1 = pos2 - pos1;
    glm::vec3 edge2 = pos3 - pos1;

    glm::vec2 deltaUV1 = uv2 - uv1;
    glm::vec2 deltaUV2 = uv3 - uv1;

    float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

    glm::vec3 tang1, bitang1;
    glm::vec3 tang2, bitang2;

    tang1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
    tang1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
    tang1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

    bitang1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
    bitang1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
    bitang1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
    
    edge1 = pos3 - pos1;
    edge2 = pos4 - pos1;
    deltaUV1 = uv3 - uv1;
    deltaUV2 = uv4 - uv1;

    f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

    tang2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
    tang2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
    tang2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

    bitang2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
    bitang2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
    bitang2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

    float wallVertices[] {
        // positions            // normal         // texcoords  // tangent                          // bitangent
        pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tang1.x, tang1.y, tang1.z, bitang1.x, bitang1.y, bitang1.z,
        pos2.x, pos2.y, pos2.z, nm.x, nm.y, nm.z, uv2.x, uv2.y, tang1.x, tang1.y, tang1.z, bitang1.x, bitang1.y, bitang1.z,
        pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tang1.x, tang1.y, tang1.z, bitang1.x, bitang1.y, bitang1.z,

        pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tang2.x, tang2.y, tang2.z, bitang2.x, bitang2.y, bitang2.z,
        pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tang2.x, tang2.y, tang2.z, bitang2.x, bitang2.y, bitang2.z,
        pos4.x, pos4.y, pos4.z, nm.x, nm.y, nm.z, uv4.x, uv4.y, tang2.x, tang2.y, tang2.z, bitang2.x, bitang2.y, bitang2.z
    };

    GLuint wallVBO, wallVAO;

    glGenVertexArrays(1, &wallVAO);
    glGenBuffers(1, &wallVBO);

    glBindVertexArray(wallVAO);

    glBindBuffer(GL_ARRAY_BUFFER, wallVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(wallVertices), wallVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);

    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));
    glEnableVertexAttribArray(4);

    GLuint diffuseTexture = loadTextureFromFile("diffuse.jpg", "Glitter/Assets/wall");
    GLuint normalTexture = loadTextureFromFile("normal.jpg", "Glitter/Assets/wall", false);

    Shader shader("Glitter/Assets/shaders");

    shader.use();
    setShaderUniforms(shader);
    glUseProgram(0);

    while (!glfwWindowShouldClose(window)) {    
        float currFrame = static_cast<float>(glfwGetTime());
        deltaTime = currFrame - lastFrame;
        lastFrame = currFrame;

        processInput(window);

        glClearColor(0.01f, 0.01f, 0.01f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();

        shader.setVec3("viewPos", camera.pos());

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, (float)glfwGetTime() * -0.1f, glm::normalize(glm::vec3(1.0, 0.0, 1.0)));

        shader.setMat4("model", model);
        shader.setMat4("projection", glm::perspective(glm::radians(camera.m_zoom), SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 100.0f));
        shader.setMat4("view", camera.viewMatrix());

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseTexture);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, normalTexture);

        shader.setInt("material.texture_diffuse1", 0);
        shader.setInt("material.texture_normal1", 1);

        shader.setBool("shouldNormalMap", normalMappingEnabled);

        glBindVertexArray(wallVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindVertexArray(0);
        glUseProgram(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return EXIT_SUCCESS;
}


void setShaderUniforms(Shader& shader) {
    shader.setMat4("model", glm::mat4(1.0f));
    shader.setMat4("projection", glm::perspective(glm::radians(camera.m_zoom), SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 100.0f));
    shader.setMat4("view", camera.viewMatrix());
    
    shader.setVec3("light.position", lightPos); 

    shader.setVec3("light.ambient", glm::vec3(0.22f));
    shader.setVec3("light.diffuse", glm::vec3(0.5f));
    shader.setVec3("light.specular", glm::vec3(1.0f));

    shader.setFloat("material.shininess", 32.0f);
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
    
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !normalMappingKeyPressed) {
        normalMappingEnabled = !normalMappingEnabled;
        normalMappingKeyPressed = true;
    }

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) 
        normalMappingKeyPressed = false;
    
}

void scrollCallback([[maybe_unused]] GLFWwindow* window, [[maybe_unused]] double xoffset, double yoffset) {
    camera.processScrollInput(yoffset);
}

