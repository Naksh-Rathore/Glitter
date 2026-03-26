#include "texture.h"

#include <glad/glad.h>
#include <stb_image.h>

#include <string>
#include <stdexcept>
#include <array>

GLuint loadTextureFromFile(const std::string& path, const std::string& directory, bool isSRGB) {
    std::string filename = path;
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);

    if (!data)
        throw std::runtime_error("Texture failed to load at path: " + filename);

    GLenum format;
    GLenum sRgbFormat;

    if (nrComponents == 1)
        format = GL_RED;
    else if (nrComponents == 3)
        format = GL_RGB;
    else if (nrComponents == 4)
        format = GL_RGBA;
    else
        throw std::runtime_error("Unsupported texture format: " + filename);

    if (isSRGB && format != GL_RED)
        sRgbFormat = format == GL_RGB ? GL_SRGB : GL_SRGB_ALPHA;

    glBindTexture(GL_TEXTURE_2D, textureID);

    if (isSRGB && format != GL_RED)
        glTexImage2D(GL_TEXTURE_2D, 0, sRgbFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    else
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);


    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
    return textureID;
}

GLuint loadCubeMapFromDirectory(const std::string& directory, const std::string& imageExtension, bool generateMipmaps, bool isSRGB) {
    std::array<std::string, 6> faces = {
        "/right" + imageExtension,
        "/left" + imageExtension,
        "/top" + imageExtension,
        "/bottom" + imageExtension,
        "/front" + imageExtension,
        "/back" + imageExtension,
    };

    GLuint textureID;

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    unsigned char *data;

    for (int i = 0; i < 6; i++) {
        data = stbi_load((directory + faces.at(i)).c_str(), &width, &height, &nrChannels, 0); 

        if (!data)
            throw std::runtime_error("Texture failed to load at path: " + directory + faces.at(i));

        GLenum format;
        GLenum sRgbFormat;

        if (nrChannels == 1)
                format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;
        else
            throw std::runtime_error("Unsupported texture format: " + directory + faces.at(i));

        if (isSRGB && format != GL_RED)
            sRgbFormat = format == GL_RGB ? GL_SRGB : GL_SRGB_ALPHA;

        if (isSRGB && format != GL_RED)
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, sRgbFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        else
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

        stbi_image_free(data);
    }

    if (generateMipmaps)
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        
    if (generateMipmaps)
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    else
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}


Texture::Texture(const std::string& imagePath, const std::string& textureName, bool isSRGB) 
    : m_imagePath(imagePath)
    , m_textureName(textureName)
{
    uploadTexture(isSRGB);
}

void Texture::uploadTexture(bool isSRGB) {
    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_2D, m_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int imageWidth;
    int imageHeight;
    int imageNrChannels;

    stbi_set_flip_vertically_on_load(true);

    unsigned char *imageData = stbi_load(m_imagePath.c_str(), &imageWidth, &imageHeight, &imageNrChannels, 0);

    if (!imageData)
        std::__throw_runtime_error("Could not open image");
    
    GLenum format;
    GLenum sRgbFormat;

    if (imageNrChannels == 1)
        format = GL_RED;
    else if (imageNrChannels == 3)
        format = GL_RGB;
    else if (imageNrChannels == 4)
        format = GL_RGBA;
    else
        throw std::runtime_error("Unsupported texture format: " + m_imagePath);

    if (isSRGB && format != GL_RED)
        sRgbFormat = format == GL_RGB ? GL_SRGB : GL_SRGB_ALPHA;

    glBindTexture(GL_TEXTURE_2D, m_id);

    if (isSRGB && format != GL_RED)
        glTexImage2D(GL_TEXTURE_2D, 0, sRgbFormat, imageWidth, imageHeight, 0, format, GL_UNSIGNED_BYTE, imageData);
    else
        glTexImage2D(GL_TEXTURE_2D, 0, format, imageWidth, imageHeight, 0, format, GL_UNSIGNED_BYTE, imageData);

    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(imageData);
}
