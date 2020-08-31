#include <src/ResourceManagement/MaterialLoader.h>

#include <string>
#include <cstring>

#include <src/ResourceManagement/ResourceManager.h>
#include <src/Graphics/OpenGL/Shader.h>
#include <src/Utils/Logger.h>

namespace DwarfQuest {
    namespace Core {

        Material* LoadMaterial(const char* path) {
            FILE* file = std::fopen(path, "r");
            if (file == NULL) {
                Logger::Error("LoadMaterial - Could not read file path: " + std::string(path));
                return NULL;
            }

            char shaderPath[128];
            int res = fscanf(file, "%s", shaderPath);
            Shader* shader = ResourceManager::GetOrLoadShaderAsset(shaderPath);
            if (shader == nullptr) {
                Logger::Error("LoadMaterial - Could not read material's shader file path: " + std::string(shaderPath));
                std::fclose(file);
                return NULL;
            }
            Material* material = new Material(shader);

            char lineHeader[128];
            char uniformName[32];
            res = fscanf(file, "%s", lineHeader);
            while (res != EOF) {
                if (strcmp(lineHeader, "float") == 0) {
                    float value;
                    res = fscanf(file, "%s %f", uniformName, &value);
                    material->SetData(uniformName, &value, sizeof(float));

                } else if (strcmp(lineHeader, "vec2") == 0) {
                    glm::vec2 value;
                    res = fscanf(file, "%s %f %f", uniformName, &value.x, &value.y);
                    material->SetData(uniformName, &value, sizeof(GLfloat) * 2);

                } else if (strcmp(lineHeader, "vec3") == 0) {
                    glm::vec3 value;
                    res = fscanf(file, "%s %f %f %f", uniformName, &value.x, &value.y, &value.z);
                    material->SetData(uniformName, &value, sizeof(GLfloat) * 3);

                } else if (strcmp(lineHeader, "vec4") == 0) {
                    glm::vec4 value;
                    res = fscanf(file, "%s %f %f %f %f", uniformName, &value.x, &value.y, &value.z, &value.w);
                    material->SetData(uniformName, &value, sizeof(GLfloat) * 4);

                } else if (strcmp(lineHeader, "int") == 0) {
                    GLint value;
                    res = fscanf(file, "%s %d", uniformName, &value);
                    material->SetData(uniformName, &value, sizeof(GLint));

                } else if (strcmp(lineHeader, "uint") == 0) {
                    GLuint value;
                    res = fscanf(file, "%s %u", uniformName, &value);
                    material->SetData(uniformName, &value, sizeof(GLuint));

                } else if (strcmp(lineHeader, "bool") == 0) {
                    bool value = false;
                    char value_str[5];
                    res = fscanf(file, "%s %s", uniformName, &value_str);
                    if (strcmp(value_str, "true") == 0) value = true;
                    material->SetData(uniformName, &value, sizeof(GLboolean));

                } else if (strcmp(lineHeader, "mat2") == 0) {
                    glm::mat2 value;
                    res = fscanf(file, "%s %f %f %f %f", uniformName, &value[0][0], &value[0][1], 
                                                                      &value[1][0], &value[1][1]);
                    material->SetData(uniformName, &value[0][0], sizeof(GLfloat) * 4);

                } else if (strcmp(lineHeader, "mat3") == 0) {
                    glm::mat3 value;
                    res = fscanf(file, "%s %f %f %f %f %f %f %f %f %f", uniformName, &value[0][0], &value[0][1], &value[0][2], 
                                                                                     &value[1][0], &value[1][1], &value[1][2], 
                                                                                     &value[2][0], &value[2][1], &value[2][1]);
                    material->SetData(uniformName, &value[0][0], sizeof(GLfloat) * 9);

                } else if (strcmp(lineHeader, "mat4") == 0) {
                    glm::mat3 value;
                    res = fscanf(file, "%s %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f", uniformName, &value[0][0], &value[0][1], &value[0][2], &value[0][3],
                                                                                                          &value[1][0], &value[1][1], &value[1][2], &value[1][3],
                                                                                                          &value[2][0], &value[2][1], &value[2][2], &value[2][3], 
                                                                                                          &value[3][0], &value[3][1], &value[3][2], &value[3][3]);
                    material->SetData(uniformName, &value[0][0], sizeof(GLfloat) * 16);

                } else if (strcmp(lineHeader, "texture2D") == 0) {
                    char texturePath[128];
                    res = fscanf(file, "%s %s", uniformName, &texturePath);
                    Texture* texture = ResourceManager::GetOrLoadTextureAsset(texturePath);
                    if (texture == nullptr) {
                        delete material;
                        Logger::Error("LoadMaterial - Could not read material's texture file path: " + std::string(texturePath));
                        std::fclose(file);
                        return NULL;
                    }
                    material->SetData(uniformName, &texture, sizeof(void *));

                } else if (strcmp(lineHeader, "cubeTexture") == 0) {
                    // Not supported yet.
                }

                res = fscanf(file, "%s", lineHeader);
            }
            std::fclose(file);
            return material;
        }

    }
}