#pragma once
#include <unordered_map>
#include <string>

#include "../Graphics/OpenGL/Mesh.h"
#include "../Graphics/OpenGL/Texture.h"
#include "../Graphics/OpenGL/Shader.h"

namespace DwarfQuest {
    namespace Core {
        // ResourceManager uses different unordered_maps to map assets' file path string to it's pointer
        class ResourceManager {
        private:
            static std::unordered_map<std::string, Mesh*> m_meshesAssetMap;
            static std::unordered_map<std::string, Texture*> m_texturesAssetMap;
            static std::unordered_map<std::string, Shader*> m_shadersAssetMap;

        public:
            static void DestroyAssets();
            // Is loaded
            static bool IsTextureAssetLoaded(const char* path);
            static bool IsMeshAssetLoaded(const char* path);
            static bool IsShaderAssetLoaded(const char* path);

            // Get
            static Texture* GetOrLoadTextureAsset(const char* path);
            static Mesh* GetOrLoadMeshAsset(const char* path);
            static Shader* GetOrLoadShaderAsset(const char* path);

            // Load
            static Texture* LoadTextureAsset(const char* path, bool reload = false);
            static Mesh* LoadMeshAsset(const char* path, bool reload = false);
            static Shader* LoadShaderAsset(const char* path, bool reload = false);

            // Unload
            static void UnloadTextureAsset(const char* path);
            static void UnloadMeshAsset(const char* path);
            static void UnloadShaderAsset(const char* path);
        };
    }
}