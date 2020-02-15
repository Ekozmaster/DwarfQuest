#pragma once

#include <GL/glew.h>

class Shader {
private:
    bool m_allocated;
    unsigned int m_shaderProgram;

private:
    void CompileShaders(const char* vertexPath, const char* fragmentPath);

public:
    Shader();
    Shader(const char* vertexPath, const char* fragmentPath);
    ~Shader();

    void Destroy();

    // Don't use these methods directly unless you know exactly what you're doing.
    int Use();
    void Set4x4Matrix(const char *matrixName, const GLfloat *values);
};