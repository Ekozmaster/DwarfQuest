#pragma once

#include "../../libs/stb_image/stb_image.h"

#include "../Graphics/OpenGL/Texture.h"

namespace DwarfQuest {
    namespace Core {

        Texture* LoadTexture(const char* path);
    }
}