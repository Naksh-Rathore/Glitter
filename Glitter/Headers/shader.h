#pragma once

#include <glad/glad.h>

#include <string>

class Shader {
    public:
        std::string m_assetsDirectoryPath;

        std::string m_vertSrc;
        std::string m_fragSrc;

        Shader(const std::string& assetsDirectoryPath);

        Shader(const Shader&) = delete;
        Shader& operator=(const Shader&) = delete;       

        GLuint vert() { return m_vert; }
        GLuint frag() { return m_frag; }
        GLuint id() { return m_id; }           
        
        void use();

    private:
        GLuint m_vert;
        GLuint m_frag;

        GLuint m_id;

        void linkShader();
        void readShaderSources();
        void checkCompileErrors(const std::string& type, GLuint shader);
};