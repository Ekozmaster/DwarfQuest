#include "Shader.h"

#include <string>
#include <fstream>
#include <sstream>
#ifdef _WIN32
#include<Windows.h>
#endif
#include<GL/glew.h>

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
}

Shader::Shader(const char* vertexPath, const char* fragmentPath) {
    CompileShaders(vertexPath, fragmentPath);
}

Shader::~Shader() {
    glUseProgram(0);
    glDeleteProgram(m_shaderProgram);
}

void Shader::Use() {
    glUseProgram(m_shaderProgram);
}