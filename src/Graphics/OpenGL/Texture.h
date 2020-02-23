#pragma once
#include <glad/glad.h>

class Texture {
private:
    bool m_allocated;
    unsigned int m_id, m_width, m_height;
    GLenum m_channels;

public:
    Texture();
    ~Texture();

    void Create(unsigned char* data, unsigned int width, unsigned int height, GLenum channels);
    void Use(GLuint slot);
    void Destroy();
};