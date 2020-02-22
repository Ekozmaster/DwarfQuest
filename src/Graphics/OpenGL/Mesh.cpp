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
    GLTrackCall(glGenVertexArrays(1, &m_vao));
    GLTrackCall(glBindVertexArray(m_vao));
    
    // Vertex buffer object
    GLTrackCall(glGenBuffers(1, &m_vbo));
    GLTrackCall(glBindBuffer(GL_ARRAY_BUFFER, m_vbo));
    GLTrackCall(glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertexArrayLength * 2, vertexArray, GL_STATIC_DRAW));
   
    // Indexes buffer objec)t
    GLTrackCall(glGenBuffers(1, &m_ibo));
    GLTrackCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo));
    GLTrackCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indexesArrayLength, indexesArray, GL_STATIC_DRAW));

    // Setting location 0 of "vertexArray" as position, 1 as color).
    GLTrackCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0)); // Position
    GLTrackCall(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)))); // Color
    GLTrackCall(glEnableVertexAttribArray(0));
    GLTrackCall(glEnableVertexAttribArray(1));

    GLTrackCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GLTrackCall(glBindVertexArray(0));
    
    m_allocated = true;
    m_verticesCount = vertexArrayLength;
    m_indexesCount = indexesArrayLength;
    
    return 0;
}

void Mesh::Destroy() {
    if (!m_allocated) return;
    
    GLTrackCall(glBindVertexArray(0));
    
    GLTrackCall(glDeleteBuffers(1, &m_vbo));
    GLTrackCall(glDeleteBuffers(1, &m_ibo));
    GLTrackCall(glDeleteVertexArrays(1, &m_vao));
    
    m_vao = m_vbo = m_ibo = 0;
    m_verticesCount = m_indexesCount = 0;
    m_allocated = false;
}

int Mesh::Use() {
    if (!m_allocated) return 1;
    GLTrackCall(glBindVertexArray(m_vao));
}

void Mesh::Render() {
    GLTrackCall(glDrawElements(GL_TRIANGLES, m_indexesCount, GL_UNSIGNED_INT, 0));
}