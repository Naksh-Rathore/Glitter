#include "mesh.h"

#include <glad/glad.h>

#include <string>
#include <vector>

#include "texture.h"
#include "shader.h"

Mesh::Mesh(const std::vector<float>& vertices, const std::vector<unsigned int>& indices,  const std::vector<Texture>& textures)
        : m_vertices(vertices)
        , m_indices(indices)
        , m_textures(textures)
{}

void Mesh::uploadMesh() {
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
  
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(float), m_vertices.data(), GL_STATIC_DRAW);  

    // vertex positions
    glEnableVertexAttribArray(0);	
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);	
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    // vertex texture coords
    glEnableVertexAttribArray(2);	
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

    if (m_indices.empty())
        return;
    
    glGenBuffers(1, &m_EBO);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), m_indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void Mesh::draw(Shader& shader) {
    shader.use();

    std::string textureUniformNumber;

    int diffuseNr = 1;
    int specularNr = 1;

    /* Format for phong lighting uniforms
                
                Texture Type    Texture Number
       material.texture_specular1;
       material.texture_diffuse1;

    */

    for (int i = 0; i < (int) m_textures.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i);

        if (m_textures.at(i).m_textureName == "texture_specular")
            textureUniformNumber = std::to_string(specularNr++);
        else if (m_textures.at(i).m_textureName == "texture_diffuse")
            textureUniformNumber = std::to_string(diffuseNr++);

        shader.setInt(("material." + m_textures.at(i).m_textureName + textureUniformNumber).c_str(), i);

        glBindTexture(GL_TEXTURE_2D, m_textures.at(i).id());
    }  

    glBindVertexArray(m_VAO);
    
    if (m_indices.empty())
        glDrawArrays(GL_TRIANGLES, 0, (int) m_vertices.size() / 8);
    else
        glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glUseProgram(0);
}
