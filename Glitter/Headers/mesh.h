#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <vector>

#include "texture.h"
#include "shader.h"

struct Vertex {
    glm::vec3 m_pos;
    glm::vec3 m_normal;
    glm::vec2 m_tex;
};

// A renderable mesh (without shader) 
// Assumes the mesh being rendered is phong lit (diffuse and specular map)
// Best used for model loading
class Mesh {
    public:
        std::vector<Vertex> m_vertices;
        std::vector<unsigned int> m_indices;
        std::vector<BasicTexture> m_textures;
        std::vector<glm::vec3> m_offsets;
        
        Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices = {}, const std::vector<BasicTexture>& textures = {}, const std::vector<glm::vec3>& offsets = {});

        void draw(Shader& shader);
        void uploadMesh();

    private:
        GLuint m_VBO;
        GLuint m_VAO;
        GLuint m_EBO;

        GLuint m_instanceVBO;
};
