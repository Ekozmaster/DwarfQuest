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

void Shader::Set4x4Matrix(const char *matrixName, const GLfloat *values) {
    GLTrackCall(GLuint matrixID = glGetUniformLocation(m_shaderProgram, matrixName));

    if (matrixID == -1) {
        std::stringstream log;
        log << "Shader.Set4x4Matrix - Shader has no matrix named '" << matrixName << "'";
        Logger::Error(log.str().c_str());
        return;
    }
    GLTrackCall(glUniformMatrix4fv(matrixID, 1, GL_FALSE, values));
}