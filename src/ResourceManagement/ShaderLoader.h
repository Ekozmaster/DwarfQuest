#pragma once

#include <src/Graphics/OpenGL/Shader.h>

namespace DwarfQuest {
    namespace Core {
        Shader* LoadShader(const char* shaderPath);
    }
}