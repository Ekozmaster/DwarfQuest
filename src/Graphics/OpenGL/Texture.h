#pragma once

#include <glad/glad.h>
#include <map>
#include <string>
#include <glm/glm.hpp>

namespace DwarfQuest {
    namespace Core {

        class Texture {
        private:
            bool m_allocated;
            unsigned int m_id, m_width, m_height;
            GLenum m_channels;

        public:
            Texture();
            ~Texture();

            void Create(unsigned char* data, unsigned int width, unsigned int height, GLenum channels, GLenum samplingMode);
            void Use(GLuint slot);
            void Destroy();
        };

        typedef struct {
            glm::vec2 top_left;
            glm::vec2 top_right;
            glm::vec2 bottom_left;
            glm::vec2 bottom_right;
        } TextureCoordinate;

        typedef struct {
            Texture* texture;
            std::map<std::string, TextureCoordinate> coordinates;
        } TextureAtlas;
    }
}
