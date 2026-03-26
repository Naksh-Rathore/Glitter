#pragma once

#include <glad/glad.h>

#include <string>

GLuint loadTextureFromFile(const std::string& path, const std::string& directory, bool isSRGB = true);

// Assumes naming convention:
// texture1.png
// texture2.png
// ...
GLuint loadCubeMapFromDirectory(const std::string& directory, const std::string& imageExtension = ".png", bool generateMipmaps = false, bool isSRGB = true);

struct BasicTexture {
    std::string m_textureName;
    GLuint m_id;
    std::string m_path;
};

class Texture {
    public:
        std::string m_imagePath;
        std::string m_textureName;

        Texture(const std::string& imagePath, const std::string& textureName, bool isSRGB = true);
 
        GLuint id() { return m_id; }

    private:
        GLuint m_id;

        void uploadTexture(bool isSRGB);
};
