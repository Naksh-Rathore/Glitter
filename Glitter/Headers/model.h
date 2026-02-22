#pragma once

#include <vector>
#include <string>

#include "mesh.h"
#include "texture.h"
#include "shader.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


class Model {
    public:
        Model(const std::string& path, bool shouldFlipUVs = false);

        void draw(Shader& shader);

    private:
        std::vector<Mesh> m_meshes;
        std::string m_modelDirectory;
        std::vector<BasicTexture> m_loadedTextures;
        
        void loadModel(const std::string& path, bool shouldFlipUVs = false); 

        void processNode(aiNode *node, const aiScene *scene);
        Mesh processMesh(aiMesh *mesh, const aiScene *scene);
        std::vector<BasicTexture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
};
