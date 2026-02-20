#pragma once

#include <glad/glad.h>

#include <string>

struct BasicTexture {
    std::string m_textureName;
    GLuint m_id;
};

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
