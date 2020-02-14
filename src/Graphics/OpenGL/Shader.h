#pragma once

class Shader {
private:
    unsigned int m_shaderProgram;

private:
    void CompileShaders(const char* vertexPath, const char* fragmentPath);

public:
    Shader(const char* vertexPath, const char* fragmentPath);
    ~Shader();

    void Use();
};