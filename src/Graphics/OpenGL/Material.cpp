#include <src/Graphics/OpenGL/Graphics.h>
#include <src/Graphics/OpenGL/Material.h>

namespace DwarfQuest {
    namespace Core {
        Material::Material(Shader* shader): m_shader(shader) {
            attributes = m_shader->GetShaderAttributes();
            uniforms = m_shader->GetShaderUniforms();
        }

        Material::~Material() {}

        int Material::Use() {
            Graphics::SetShader(m_shader);
            // Graphics::SetTexture(...);
            return 0;
        }
    }

}