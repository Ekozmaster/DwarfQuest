#pragma once

#include <src/Graphics/OpenGL/Shader.h>

namespace DwarfQuest {
    namespace Core {
        // Table with data types:
        // https://www.khronos.org/opengl/wiki/GLAPI/glGetActiveUniforms

        class Material {
            Shader* m_shader;

        public:
            std::vector<ShaderAttribute> attributes;
            std::vector<ShaderUniform> uniforms;

            Material(Shader* shader);
            ~Material();

            // Don't use these methods directly unless you know exactly what you're doing.
            int Use();
        };
    }
}
