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
        
        Mesh (const Mesh&) = delete;
        Mesh& operator=(const Mesh&) = delete;

        Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices = {}, const std::vector<BasicTexture>& textures = {});

        void draw(Shader& shader);
        void uploadMesh();

    private:
        GLuint m_VBO;
        GLuint m_VAO;
        GLuint m_EBO;
};
