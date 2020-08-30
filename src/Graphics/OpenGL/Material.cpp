#include <src/Graphics/OpenGL/Material.h>

#include<string>
#include<vector>

#include <src/Graphics/OpenGL/Graphics.h>
#include <src/Graphics/OpenGL/Utils.h>

namespace DwarfQuest {
    namespace Core {
        Material::Material(Shader* shader): m_shader(shader) {
            // Getting relevant/non-global uniforms for the material.
            std::vector<ShaderUniform> shaderUniforms = m_shader->GetShaderUniforms();
            std::vector<std::string> unrelevantUniforms = Graphics::graphicsUniformsNames;
            for (auto it = shaderUniforms.begin(); it != shaderUniforms.end(); ++it) {
                if (std::find(unrelevantUniforms.begin(), unrelevantUniforms.end(), it->name) == unrelevantUniforms.end()) {
                    uniforms.push_back(*it);
                }
            }

            // Allocating memory to store data for all uniforms.
            for (auto it = uniforms.begin(); it != uniforms.end(); ++it) it->dataLocation = malloc(it->dataSize);
        }

        Material::~Material() {
            FreeUniformsDataMemory();
        }

        void Material::FreeUniformsDataMemory() {
            for (auto it = uniforms.begin(); it != uniforms.end(); ++it) {
                if (it->dataLocation != nullptr) free(it->dataLocation);
            }
        }

        void Material::SetData(const std::string& name, const void* data, size_t size) {
            for (auto it = uniforms.begin(); it != uniforms.end(); ++it) {
                if (it->name == name) {
                    memcpy(it->dataLocation, data, size);
                    break;
                }
            }
        }

        int Material::Use() {
            Graphics::SetShader(m_shader);

            unsigned int textureSlot = 0;
            for (auto it = uniforms.begin(); it != uniforms.end(); ++it) {
                switch (it->type) {
                case GL_FLOAT:
                    m_shader->SetFloatUniform(it->name.c_str(), *(GLfloat*)(it->dataLocation));
                    break;
                case GL_FLOAT_VEC2:
                    m_shader->SetVec2Uniform(it->name.c_str(), (GLfloat*)(it->dataLocation));
                    break;
                case GL_FLOAT_VEC3:
                    m_shader->SetVec3Uniform(it->name.c_str(), (GLfloat*)(it->dataLocation));
                    break;
                case GL_FLOAT_VEC4:
                    m_shader->SetVec4Uniform(it->name.c_str(), (GLfloat*)(it->dataLocation));
                    break;
                case GL_INT:
                    m_shader->SetIntUniform(it->name.c_str(), *(GLint*)(it->dataLocation));
                    break;
                case GL_UNSIGNED_INT:
                    m_shader->SetUintUniform(it->name.c_str(), *(GLuint*)(it->dataLocation));
                    break;
                case GL_BOOL:
                    m_shader->SetBoolUniform(it->name.c_str(), *(GLboolean*)(it->dataLocation));
                    break;
                case GL_FLOAT_MAT2:
                    m_shader->Set2x2MatrixUniform(it->name.c_str(), (GLfloat*)(it->dataLocation));
                    break;
                case GL_FLOAT_MAT3:
                    m_shader->Set3x3MatrixUniform(it->name.c_str(), (GLfloat*)(it->dataLocation));
                    break;
                case GL_FLOAT_MAT4:
                    m_shader->Set4x4MatrixUniform(it->name.c_str(), (GLfloat*)(it->dataLocation));
                    break;
                case GL_SAMPLER_2D:
                    Texture* texture = (Texture*)(it->dataLocation);
                    Graphics::SetTexture(texture, textureSlot);
                    m_shader->SetIntUniform(it->name.c_str(), textureSlot);
                    textureSlot++;
                    break;
                case GL_SAMPLER_CUBE:
                    // Not suppoerted yet.
                    break;
                }
            }
            return 0;
        }
    }

}