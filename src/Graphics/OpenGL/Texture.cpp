#include "Texture.h"
#include "Utils.h"

namespace DwarfQuest {
    namespace Core {

        Texture::Texture() {
            m_allocated = false;
            m_id = 0;
            m_width = 0;
            m_height = 0;
            m_channels = GL_RGB;
        }

        Texture::~Texture() {
            Destroy();
        }

        void Texture::Create(unsigned char* data, unsigned int width, unsigned int height, GLenum channels, GLenum samplingMode) {
            Destroy();

            GLTrackCall(glGenTextures(1, &m_id));
            GLTrackCall(glBindTexture(GL_TEXTURE_2D, m_id));

            GLTrackCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
            GLTrackCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
            GLTrackCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, samplingMode));
            GLTrackCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, samplingMode));

            GLTrackCall(glTexImage2D(GL_TEXTURE_2D, 0, channels, width, height, 0, channels, GL_UNSIGNED_BYTE, data));
            GLTrackCall(glGenerateMipmap(GL_TEXTURE_2D));

            GLTrackCall(glBindTexture(GL_TEXTURE_2D, 0));

            m_width = width;
            m_height = height;
            m_channels = channels;
            m_allocated = true;
        }

        void Texture::Use(unsigned int slot) {
            GLTrackCall(glActiveTexture(GL_TEXTURE0 + slot));
            GLTrackCall(glBindTexture(GL_TEXTURE_2D, m_id));
        }

        void Texture::Destroy() {
            if (!m_allocated) return;

            GLTrackCall(glDeleteTextures(1, &m_id));
            m_id = 0;
            m_width = 0;
            m_height = 0;

            m_allocated = false;
        }

    }
}