#include "shader.h"

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <iostream>

Shader::Shader(const std::string& assetsDirectoryPath)
    : m_assetsDirectoryPath(assetsDirectoryPath)

    , m_vert(glCreateShader(GL_VERTEX_SHADER))
    , m_frag(glCreateShader(GL_FRAGMENT_SHADER))
    , m_id(glCreateProgram())
{
    linkShader();
}

void Shader::readShaderSources() {
    std::ifstream vert(m_assetsDirectoryPath + "/vertex.vs");
    std::ifstream frag(m_assetsDirectoryPath + "/fragment.fs");

    std::string readBuffer;

    while (std::getline(vert, readBuffer))
        m_vertSrc += readBuffer + "\n";

    while (std::getline(frag, readBuffer))
        m_fragSrc += readBuffer + "\n";

    vert.close();
    frag.close();
}

void Shader::linkShader() {
    readShaderSources();

    const char *vertSrcPtr = m_vertSrc.c_str(); 
    const char *fragSrcPtr = m_fragSrc.c_str(); 

    glShaderSource(m_vert, 1, &vertSrcPtr, nullptr);
    glCompileShader(m_vert);
    checkCompileErrors("VERTEX", m_vert);

    glShaderSource(m_frag, 1, &fragSrcPtr, nullptr);
    glCompileShader(m_frag);
    checkCompileErrors("FRAGMENT", m_frag);
    
    glAttachShader(m_id, m_vert);
    glAttachShader(m_id, m_frag);
    
    glLinkProgram(m_id);
    checkCompileErrors("PROGRAM", m_id);

    glDeleteShader(m_vert);
    glDeleteShader(m_frag);
}

void Shader::checkCompileErrors(const std::string& type, GLuint shader) {
    int success;
    char infoLog[1024];
    if (type != "PROGRAM") {

        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }   
}

void Shader::use() {
    glUseProgram(m_id);
}