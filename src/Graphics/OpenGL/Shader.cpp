#include "Shader.h"

#include <string>
#include <fstream>
#include <sstream>
#ifdef _WIN32
#include<Windows.h>
#endif
#include <glad/glad.h>

#include "../../Utils/Logger.h"
#include "Utils.h"
#include <vector>

namespace DwarfQuest {
    namespace Core {

        Shader::Shader() {
            m_shaderProgram = 0;
            m_allocated = false;
        }

        Shader::~Shader() {
            Destroy();
        }

        bool Shader::CompileShaders(const char* shaderName, const char* vertexSource, const char* fragmentSource) {
            int success = 0;
            char infoLog[512];

            // VERTEX
            GLTrackCall(unsigned int vertex = glCreateShader(GL_VERTEX_SHADER));
            GLTrackCall(glShaderSource(vertex, 1, &vertexSource, NULL));
            GLTrackCall(glCompileShader(vertex));

            GLTrackCall(glGetShaderiv(vertex, GL_COMPILE_STATUS, &success));
            if (!success) {
                GLTrackCall(glGetShaderInfoLog(vertex, 512, NULL, infoLog));
                Logger::Error("Shader.CompileShaders - Could not compile VERTEX shader('" + std::string(shaderName) + "'):\n" + infoLog);
                GLTrackCall(glDeleteShader(vertex));
                return false;
            };

            // FRAGMENT
            GLTrackCall(unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER));
            GLTrackCall(glShaderSource(fragment, 1, &fragmentSource, NULL));
            GLTrackCall(glCompileShader(fragment));

            GLTrackCall(glGetShaderiv(fragment, GL_COMPILE_STATUS, &success));
            if (!success) {
                GLTrackCall(glGetShaderInfoLog(fragment, 512, NULL, infoLog));
                Logger::Error("Shader.CompileShaders - Could not compile FRAGMENT shader('" + std::string(shaderName) + "'):\n" + infoLog);
                GLTrackCall(glDeleteShader(vertex));
                GLTrackCall(glDeleteShader(fragment));
                return false;
            };

            // FINAL PROGRAM
            GLTrackCall(m_shaderProgram = glCreateProgram());
            GLTrackCall(glAttachShader(m_shaderProgram, vertex));
            GLTrackCall(glAttachShader(m_shaderProgram, fragment));
            GLTrackCall(glLinkProgram(m_shaderProgram));

            GLTrackCall(glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, &success));
            if (!success) {
                GLTrackCall(glGetProgramInfoLog(m_shaderProgram, 512, NULL, infoLog));
                Logger::Error("Shader.CompileShaders - Could not link shader('" + std::string(shaderName) + "'):\n" + infoLog);
                GLTrackCall(glDeleteShader(vertex));
                GLTrackCall(glDeleteShader(fragment));
                GLTrackCall(glDeleteProgram(m_shaderProgram));
                return false;
            }

            GLTrackCall(glDeleteShader(vertex));
            GLTrackCall(glDeleteShader(fragment));

            m_allocated = true;
            return true;
        }

        void Shader::Set4x4Matrix(const char* matrixName, const GLfloat* values) {
            GLTrackCall(GLuint matrixID = glGetUniformLocation(m_shaderProgram, matrixName));

            if (matrixID == -1) {
                std::stringstream log;
                log << "Shader.Set4x4Matrix - Shader has no matrix named '" << matrixName << "'";
                Logger::Error(log.str().c_str());
                return;
            }
            GLTrackCall(glUniformMatrix4fv(matrixID, 1, GL_FALSE, values));
        }

        std::vector<ShaderAttribute> Shader::GetShaderAttributes() {
            GLint numActiveAttribs = 0;
            GLTrackCall(glGetProgramiv(m_shaderProgram, GL_ACTIVE_ATTRIBUTES, &numActiveAttribs));
            std::vector<ShaderAttribute> result;

            GLint maxAttribNameLength = 0;
            GLTrackCall(glGetProgramiv(m_shaderProgram, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxAttribNameLength));
            char *name = new char[maxAttribNameLength];
            for (int attrib = 0; attrib < numActiveAttribs; ++attrib) {
                GLint size = 0;
                GLenum type = 0;
                GLsizei length = 0;
                GLTrackCall(glGetActiveAttrib(m_shaderProgram, attrib, maxAttribNameLength, &length, &size, &type, name));
                std::string attribName(name, length);
                result.push_back(ShaderAttribute({ type, attribName }));
            }
            delete[] name;
            return result;
        }

        std::vector<ShaderUniform> Shader::GetShaderUniforms() {
            GLint numActiveUniforms = 0;
            GLTrackCall(glGetProgramiv(m_shaderProgram, GL_ACTIVE_UNIFORMS, &numActiveUniforms));
            std::vector<ShaderUniform> result;

            char* name = new char[256];
            for (int unif = 0; unif < numActiveUniforms; ++unif) {
                GLint size = 0;
                GLenum type = 0;
                GLsizei length = 0;
                GLTrackCall(glGetActiveUniform(m_shaderProgram, unif, 256, &length, &size, &type, name));
                std::string unifName(name, length);
                result.push_back(ShaderUniform({ type, unifName }));
            }

            delete[] name;
            return result;
        }

        void Shader::Destroy() {
            if (!m_allocated) return;

            GLTrackCall(glUseProgram(0));
            GLTrackCall(glDeleteProgram(m_shaderProgram));
            m_shaderProgram = 0;

            m_allocated = false;
        }

        int Shader::Use() {
            if (!m_allocated) return 1;
            glUseProgram(m_shaderProgram);
            return 0;
        }
    }
}