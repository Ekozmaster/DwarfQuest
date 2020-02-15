#include "Mesh.h"

Mesh::Mesh() {
    m_allocated = false;
}

Mesh::~Mesh() {
    DestroyMesh();
}

int Mesh::CreateMesh(const GLfloat *vertexArray, const GLuint vertexArrayLength, const GLuint *indexesArray, const GLuint indexesArrayLength) {
    if (m_allocated) DestroyMesh();
    
    // Vertex array object
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);
     
    // Vertex buffer object
    glGenBuffers(1, &m_vbo);  
    glBindBuffer(GL_VERTEX_ARRAY, m_vbo);  
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertexArrayLength, vertexArray, GL_STATIC_DRAW);
    
    // Indexes buffer object
    glGenBuffers(1, &m_ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indexesArrayLength, indexesArray, GL_STATIC_DRAW);
    
    m_allocated = true;
    
    return 0;
}

void Mesh::DestroyMesh() {
    if (!m_allocated) return;
    
    glBindVertexArray(0);
    
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_ibo);
    glDeleteVertexArrays(1, &m_vao);
    
    m_vao = 0;
    m_vbo = 0;
    m_ibo = 0;
    m_allocated = false;
}

int Mesh::UseMesh() {
    if (!m_allocated) return 1;
    
    glBindVertexArray(m_vao);
}