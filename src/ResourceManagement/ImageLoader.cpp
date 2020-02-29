#include <src/ResourceManagement/ImageLoader.h>

namespace DwarfQuest {
    namespace Core {

        Texture* LoadTexture(const char* path) {
            int width, height, nrChannels;
            stbi_set_flip_vertically_on_load(true);
            unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
            Texture* texture = new Texture();

            texture->Create(data, width, height, nrChannels > 3 ? GL_RGBA : GL_RGB);
            stbi_image_free(data);
            return texture;
        }

    }
}