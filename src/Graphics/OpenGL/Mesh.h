#pragma once

#include <glad/glad.h>

class Mesh {
private:
    bool m_allocated;
    GLuint m_vao, m_vbo, m_ibo;
    GLuint m_verticesCount, m_indexesCount;
    
public:
    Mesh();
    ~Mesh();
    
    int Create(const GLfloat *vertexArray, const GLuint vertexArrayLength, const GLuint *indexesArray, const GLuint indexesArrayLength);
    void Destroy();
    
    // Don't use these methods directly unless you know exactly what you're doing.
    int Use();
    void Render();
};