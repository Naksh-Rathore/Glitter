#pragma once

#include <glad/glad.h>

#include <string>

class Texture {
    public:
        std::string m_imagePath;
        std::string m_textureName;

        Texture(const std::string& imagePath, const std::string& textureName);
 
        GLuint id() { return m_id; }

    private:
        GLuint m_id;

        void uploadTexture();
};
