#pragma once

#include <libs/stb_image/stb_image.h>

#include <src/Graphics/OpenGL/Texture.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>

namespace Core {
    Texture* LoadTexture(const char* path);
    TextureAtlas* LoadTextureAtlas(std::vector<std::string> texturesPaths);
}

