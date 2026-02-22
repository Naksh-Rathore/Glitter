#include "model.h"

#include <stdexcept>
#include <vector>
#include <string>
#include <stdexcept>
#include <cstring>

#include "assimp/material.h"
#include "mesh.h"
#include "texture.h"
#include "shader.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

Model::Model(const std::string& path, bool flipUVs)
{
    loadModel(path, flipUVs);
}

void Model::draw(Shader &shader) {
    for (int i = 0; i < (int) m_meshes.size(); i++)
        m_meshes.at(i).draw(shader);
}

void Model::loadModel(const std::string& path, bool flipUVs) {
    Assimp::Importer import;

    unsigned int flags = flipUVs ? aiProcess_Triangulate | aiProcess_FlipUVs : aiProcess_Triangulate;

    const aiScene *scene = import.ReadFile(path, flags); 

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        throw std::runtime_error(std::string("Error::Assimp::") + import.GetErrorString());

    m_modelDirectory = path.substr(0, path.find_last_of('/'));

    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene) {
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];

        m_meshes.push_back(processMesh(mesh, scene));
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++)
        processNode(node->mChildren[i], scene);
}


Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<BasicTexture> textures;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;

        vertex.m_pos = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);

        if (mesh->HasNormals())
            vertex.m_normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z); 

        if (mesh->HasTextureCoords(0))
            vertex.m_tex = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        else
            vertex.m_tex = glm::vec2(0.0f);

        vertices.push_back(vertex);
    }
    
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];

        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

    std::vector<BasicTexture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

    std::vector<BasicTexture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

    return Mesh(vertices, indices, textures);
}

std::vector<BasicTexture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName) {
    std::vector<BasicTexture> textures;

    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;

        mat->GetTexture(type, i, &str);

        bool skipTextureLoad = false;

        for (unsigned int j = 0; j < (unsigned int) m_loadedTextures.size(); j++) {
            if (std::strcmp(m_loadedTextures.at(j).m_path.data(), str.C_Str()) == 0) {
                textures.push_back(m_loadedTextures.at(j));
                skipTextureLoad = true;
                break;
            }
        }

        if (!skipTextureLoad) {
            BasicTexture texture;

            texture.m_id = loadTextureFromFile(str.C_Str(), m_modelDirectory);
            texture.m_textureName = typeName;
            texture.m_path = str.C_Str();

            textures.push_back(texture);
            m_loadedTextures.push_back(texture);
        }
    }

    return textures;
}
