#include "Shader.h"

#include <string>
#include <fstream>
#include <sstream>
#ifdef _WIN32
#include<Windows.h>
#endif
#include <glad/glad.h>

#include "../../Utils/Logger.h"

void Shader::CompileShaders(const char* vertexPath, const char* fragmentPath) {
    std::string vCode;
    std::string fCode;
    std::ifstream vFile;
    std::ifstream fFile;
    vFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        vFile.open(vertexPath);
        fFile.open(fragmentPath);
        std::stringstream vStream, fStream;
        vStream << vFile.rdbuf();
        fStream << fFile.rdbuf();
        vFile.close();
        fFile.close();
        vCode = vStream.str();
        fCode = fStream.str();
    } catch (std::ifstream::failure e) {
        std::stringstream log;
        log << "Shader.CompileShaders - Could not read the files: '" << vertexPath << "' '" << fragmentPath << "'";
        Logger::Error(log.str().c_str());
        return;
    }

    const char* vCodePointer = vCode.c_str();
    const char* fCodePointer = fCode.c_str();
    int success = 0;
    char infoLog[512];

    // VERTEX
    unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vCodePointer, NULL);
    glCompileShader(vertex);

    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::stringstream log;
        log << "Shader.CompileShaders - Could not compile vertex shader('" << vertexPath << "'):\n" << infoLog;
        Logger::Error(log.str().c_str());
        return;
    };

    // FRAGMENT
    unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fCodePointer, NULL);
    glCompileShader(fragment);

    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        std::stringstream log;
        log << "Shader.CompileShaders - Could not compile fragment shader('" << fragmentPath << "'):\n" << infoLog;
        Logger::Error(log.str().c_str());
        glDeleteShader(vertex);
        return;
    };

    // FINAL PROGRAM
    m_shaderProgram = glCreateProgram();
    glAttachShader(m_shaderProgram, vertex);
    glAttachShader(m_shaderProgram, fragment);
    glLinkProgram(m_shaderProgram);

    glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(m_shaderProgram, 512, NULL, infoLog);
        std::stringstream log;
        log << "Shader.CompileShaders - Could not link shader('" << vertexPath << "', '" << fragmentPath << "'):\n" << infoLog;
        Logger::Error(log.str().c_str());
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        glDeleteProgram(m_shaderProgram);
        return;
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    m_allocated = true;
}

Shader::Shader(const char* vertexPath, const char* fragmentPath) {
    m_shaderProgram = 0;
    m_allocated = false;
    CompileShaders(vertexPath, fragmentPath);
}

Shader::Shader() {
    m_shaderProgram = 0;
    m_allocated = false;
}

Shader::~Shader() {
    Destroy();
}

void Shader::Destroy() {
    if (!m_allocated) return;

    glUseProgram(0);
    glDeleteProgram(m_shaderProgram);
    m_shaderProgram = 0;
}

int Shader::Use() {
    if (!m_allocated) return 1;
    glUseProgram(m_shaderProgram);
    return 0;
}

void Shader::Set4x4Matrix(const char *matrixName, const GLfloat *values) {
    GLuint matrixID = glGetUniformLocation(m_shaderProgram, matrixName);

    if (matrixID == -1) {
        std::stringstream log;
        log << "Shader.Set4x4Matrix - Shader has no matrix named '" << matrixName << "'";
        Logger::Error(log.str().c_str());
        return;
    }
    glUniformMatrix4fv(matrixID, 1, GL_FALSE, values);
}