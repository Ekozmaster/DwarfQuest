#pragma once

#include <GL/glew.h>

class Mesh {
private:
    bool m_allocated;
    GLuint m_vao, m_vbo, m_ibo;
    
public:
    Mesh();
    ~Mesh();
    
    int CreateMesh(const GLfloat *vertexArray, const GLuint vertexArrayLength, const GLuint *indexesArray, const GLuint indexesArrayLength);
    void DestroyMesh();
    
    int UseMesh();
    
};