#include "Mesh.h"
#include "Utils.h"

Mesh::Mesh() {
    m_allocated = false;
    m_vao = m_vbo = m_ibo = 0;
    m_verticesCount = m_indexesCount = 0;
}

Mesh::~Mesh() {
    Destroy();
}

int Mesh::Create(const GLfloat *vertexArray, const GLuint vertexArrayLength, const GLuint *indexesArray, const GLuint indexesArrayLength) {
    if (m_allocated) Destroy();
    
    // Vertex array object
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);
    
    // Vertex buffer object
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertexArrayLength * 2, vertexArray, GL_STATIC_DRAW);
    
    // Indexes buffer object
    glGenBuffers(1, &m_ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indexesArrayLength, indexesArray, GL_STATIC_DRAW);

    // Setting location 0 of "vertexArray" as position, 1 as color.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0); // Position
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat))); // Color
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    m_allocated = true;
    m_verticesCount = vertexArrayLength;
    m_indexesCount = indexesArrayLength;
    
    return 0;
}

void Mesh::Destroy() {
    if (!m_allocated) return;
    
    glBindVertexArray(0);
    
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_ibo);
    glDeleteVertexArrays(1, &m_vao);
    
    m_vao = m_vbo = m_ibo = 0;
    m_verticesCount = m_indexesCount = 0;
    m_allocated = false;
}

int Mesh::Use() {
    if (!m_allocated) return 1;
    glBindVertexArray(m_vao);
}

void Mesh::Render() {
    GLTrackCall(glDrawElements(GL_TRIANGLES, m_indexesCount, GL_UNSIGNED_INT, 0));
}