#pragma once

#include <glad/glad.h>

class Shader {
private:
    bool m_allocated;
    GLuint m_shaderProgram;

public:
    bool CompileShaders(const char* shaderName, const char* vertexSource, const char* fragmentSource);
    Shader();
    ~Shader();

    void Destroy();

    // Don't use these methods directly unless you know exactly what you're doing.
    int Use();
    void Set4x4Matrix(const char *matrixName, const GLfloat *values);
};