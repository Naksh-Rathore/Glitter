#pragma once

#include <glad/glad.h>

#include <string>

class Texture {
    public:
        std::string m_imagePath;

        Texture(const std::string& imagePath);
        
        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;   

        GLuint id() { return m_id; }

    private:
        GLuint m_id;

        void uploadTexture();
};