#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

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

        void setMat4(const char* name, const glm::mat4& mat) const { 
            glUniformMatrix4fv(glGetUniformLocation(m_id, name), 1, GL_FALSE, &mat[0][0]); 
        }

        void setVec3(const char* name, const glm::vec3& vec) const { 
            glUniform3fv(glGetUniformLocation(m_id, name), 1, &vec[0]); 
        }

        void setFloat(const char* name, float value) const { 
            glUniform1f(glGetUniformLocation(m_id, name), value); 
        }

        void setInt(const char* name, int value) const { 
            glUniform1i(glGetUniformLocation(m_id, name), value); 
        }

    private:
        GLuint m_vert;
        GLuint m_frag;

        GLuint m_id;

        void linkShader();
        void readShaderSources();
        void checkCompileErrors(const std::string& type, GLuint shader);
};