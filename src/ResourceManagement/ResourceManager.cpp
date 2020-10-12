#include <src/ResourceManagement/ResourceManager.h>
#include <src/ResourceManagement/ImageLoader.h>
#include <src/ResourceManagement/MeshLoader.h>
#include <src/ResourceManagement/ShaderLoader.h>
#include <src/ResourceManagement/MaterialLoader.h>
#include <src/Utils/Logger.h>

namespace DwarfQuest {
    namespace Core {

        void ResourceManager::DestroyAssets() {
            for (std::pair<std::string, Texture*> it : m_texturesAssetMap) delete it.second;
            for (std::pair<std::string, Mesh*> it : m_meshesAssetMap) delete it.second;
            for (std::pair<std::string, Shader*> it : m_shadersAssetMap) delete it.second;
            m_texturesAssetMap.clear();
            m_meshesAssetMap.clear();
            m_shadersAssetMap.clear();
        }

        // Is loaded
        bool ResourceManager::IsTextureAssetLoaded(const char* path) {
            return m_texturesAssetMap.find(path) != m_texturesAssetMap.end();
        }

        bool ResourceManager::IsMeshAssetLoaded(const char* path) {
            return m_meshesAssetMap.find(path) != m_meshesAssetMap.end();
        }

        bool ResourceManager::IsShaderAssetLoaded(const char* path) {
            return m_shadersAssetMap.find(path) != m_shadersAssetMap.end();
        }

        bool ResourceManager::IsMaterialAssetLoaded(const char* path) {
            return m_materialsAssetMap.find(path) != m_materialsAssetMap.end();
        }

        // Create
        void ResourceManager::CreateTextureAsset(const char* path, Texture* texture) {
            m_texturesAssetMap[path] = texture;
        }

        // Get
        Texture* ResourceManager::GetOrLoadTextureAsset(const char* path) {
            auto it = m_texturesAssetMap.find(path);
            if (it == m_texturesAssetMap.end()) return LoadTextureAsset(path);
            return it->second;
        }

        Mesh* ResourceManager::GetOrLoadMeshAsset(const char* path) {
            auto it = m_meshesAssetMap.find(path);
            if (it == m_meshesAssetMap.end()) return LoadMeshAsset(path);
            return it->second;
        }

        Shader* ResourceManager::GetOrLoadShaderAsset(const char* path) {
            auto it = m_shadersAssetMap.find(path);
            if (it == m_shadersAssetMap.end()) return LoadShaderAsset(path);
            return it->second;
        }

        Material* ResourceManager::GetOrLoadMaterialAsset(const char* path) {
            auto it = m_materialsAssetMap.find(path);
            if (it == m_materialsAssetMap.end()) return LoadMaterialAsset(path);
            return it->second;
        }

        // Load
        Texture* ResourceManager::LoadTextureAsset(const char* path, bool reload) {
            if (reload) UnloadTextureAsset(path);
            else {
                auto it = m_texturesAssetMap.find(path);
                if (it != m_texturesAssetMap.end()) return it->second;
            }

            Texture* texture = LoadTexture(path);
            if (texture) {
                m_texturesAssetMap[path] = texture;
                return texture;
            } else {
                Logger::Error("ResourceManager.LoadTextureAsset - Could not load Texture asset: '" + std::string(path) + "'");
            }
            return NULL;
        }

        Mesh* ResourceManager::LoadMeshAsset(const char* path, bool reload) {
            if (reload) UnloadMeshAsset(path);
            else {
                auto it = m_meshesAssetMap.find(path);
                if (it != m_meshesAssetMap.end()) return it->second;
            }

            Mesh* mesh = LoadMesh(path);
            if (mesh) {
                m_meshesAssetMap[path] = mesh;
                return mesh;
            } else {
                Logger::Error("ResourceManager.LoadMeshAsset - Could not load Mesh asset: '" + std::string(path) + "'");
            }
            return NULL;
        }

        Shader* ResourceManager::LoadShaderAsset(const char* path, bool reload) {
            if (reload) UnloadShaderAsset(path);
            else {
                auto it = m_shadersAssetMap.find(path);
                if (it != m_shadersAssetMap.end()) return it->second;
            }

            Shader* shader = LoadShader(path);
            if (shader) {
                m_shadersAssetMap[path] = shader;
                return shader;
            } else {
                Logger::Error("ResourceManager.LoadShaderAsset - Could not load Shader asset: '" + std::string(path) + "'");
            }
            return NULL;
        }

        Material* ResourceManager::LoadMaterialAsset(const char* path, bool reload) {
            if (reload) UnloadShaderAsset(path);
            else {
                auto it = m_materialsAssetMap.find(path);
                if (it != m_materialsAssetMap.end()) return it->second;
            }

            Material* material = LoadMaterial(path);
            if (material) {
                m_materialsAssetMap[path] = material;
                return material;
            } else {
                Logger::Error("ResourceManager.LoadMaterialAsset - Could not load Material asset: '" + std::string(path) + "'");
            }
            return NULL;
        }

        // Unload
        void ResourceManager::UnloadTextureAsset(const char* path) {
            auto it = m_texturesAssetMap.find(path);
            if (it == m_texturesAssetMap.end()) return;

            delete it->second;
            m_texturesAssetMap.erase(it);
        }

        void ResourceManager::UnloadMeshAsset(const char* path) {
            auto it = m_meshesAssetMap.find(path);
            if (it == m_meshesAssetMap.end()) return;

            delete it->second;
            m_meshesAssetMap.erase(it);
        }

        void ResourceManager::UnloadShaderAsset(const char* path) {
            auto it = m_shadersAssetMap.find(path);
            if (it == m_shadersAssetMap.end()) return;

            delete it->second;
            m_shadersAssetMap.erase(it);
        }

        void ResourceManager::UnloadMaterialAsset(const char* path) {
            auto it = m_materialsAssetMap.find(path);
            if (it == m_materialsAssetMap.end()) return;

            delete it->second;
            m_materialsAssetMap.erase(it);
        }

        std::unordered_map<std::string, Mesh*> ResourceManager::m_meshesAssetMap = std::unordered_map<std::string, Mesh*>();
        std::unordered_map<std::string, Texture*> ResourceManager::m_texturesAssetMap = std::unordered_map<std::string, Texture*>();
        std::unordered_map<std::string, Shader*> ResourceManager::m_shadersAssetMap = std::unordered_map<std::string, Shader*>();
        std::unordered_map<std::string, Material*> ResourceManager::m_materialsAssetMap = std::unordered_map<std::string, Material*>();

    }
}