#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

namespace Core {

#define FLOATS_PER_VERTEX 11

    class Vertex {
    public:
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 uv;
        glm::vec3 color;

        Vertex();
        Vertex(glm::vec3 pos, glm::vec3 norm, glm::vec2 uv, glm::vec3 color);
    };

    class Mesh {
    private:
        bool m_allocated;
        GLuint m_vao, m_vbo, m_ibo;
        GLuint m_verticesCount, m_indexesCount;

    public:
        Mesh();
        ~Mesh();

        int Create(const GLfloat* vertexArray, const GLuint verticesCount, const GLuint* indexesArray, const GLuint indicesCount);
        void Destroy();

        // Don't use these methods directly unless you know exactly what you're doing.
        int Use();
        void Draw();
    };
}
