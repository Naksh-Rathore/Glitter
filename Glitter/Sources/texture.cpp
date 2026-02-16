#include "texture.h"

#include <glad/glad.h>
#include <stb_image.h>

#include <string>

Texture::Texture(const std::string& imagePath) 
    : m_imagePath(imagePath)
{
    uploadTexture();
}

void Texture::uploadTexture() {
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
    
    GLenum format = imageNrChannels == 4 ? GL_RGBA : GL_RGB;

    glTexImage2D(GL_TEXTURE_2D, 0, format, imageWidth, imageHeight, 0, format, GL_UNSIGNED_BYTE, imageData);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(imageData);
}