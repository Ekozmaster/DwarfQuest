#include "Mesh.h"
#include "Utils.h"

namespace Core {

    Vertex::Vertex() : position({ 0.0 }), normal({ 0.0, 1.0, 0.0 }), uv({ 0.0, 0.0 }) {}
    Vertex::Vertex(glm::vec3 pos, glm::vec3 norm, glm::vec2 uv, glm::vec3 color) 
        : position(pos), normal(norm), uv(uv), color(color) {}

    Mesh::Mesh() {
        m_allocated = false;
        m_vao = m_vbo = m_ibo = 0;
        m_verticesCount = m_indexesCount = 0;
    }

    Mesh::~Mesh() {
        Destroy();
    }

    int Mesh::Create(const GLfloat* vertexArray, const GLuint verticesCount, const GLuint* indexesArray, const GLuint indicesCount) {
        if (m_allocated) Destroy();

        // Vertex array object
        GLTrackCall(glGenVertexArrays(1, &m_vao));
        GLTrackCall(glBindVertexArray(m_vao));

        // Vertex buffer object
        GLTrackCall(glGenBuffers(1, &m_vbo));
        GLTrackCall(glBindBuffer(GL_ARRAY_BUFFER, m_vbo));
        GLTrackCall(glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * verticesCount * FLOATS_PER_VERTEX, vertexArray, GL_STATIC_DRAW));

        // Indexes buffer objec)t
        GLTrackCall(glGenBuffers(1, &m_ibo));
        GLTrackCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo));
        GLTrackCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indicesCount, indexesArray, GL_STATIC_DRAW));

        // Setting location 0 of "vertexArray" as position, 1 as color).
        GLTrackCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, FLOATS_PER_VERTEX * sizeof(GLfloat), (void*)offsetof(Vertex, position))); // Position
        GLTrackCall(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, FLOATS_PER_VERTEX * sizeof(GLfloat), (void*)offsetof(Vertex, normal))); // Normal
        GLTrackCall(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, FLOATS_PER_VERTEX * sizeof(GLfloat), (void*)offsetof(Vertex, uv))); // UVs
        GLTrackCall(glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, FLOATS_PER_VERTEX * sizeof(GLfloat), (void*)offsetof(Vertex, color))); // UVs
        GLTrackCall(glEnableVertexAttribArray(0));
        GLTrackCall(glEnableVertexAttribArray(1));
        GLTrackCall(glEnableVertexAttribArray(2));
        GLTrackCall(glEnableVertexAttribArray(3));

        GLTrackCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
        GLTrackCall(glBindVertexArray(0));

        m_allocated = true;
        m_verticesCount = verticesCount;
        m_indexesCount = indicesCount;

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
        return 0;
    }

    void Mesh::Draw() {
        GLTrackCall(glDrawElements(GL_TRIANGLES, m_indexesCount, GL_UNSIGNED_INT, 0));
    }

}
