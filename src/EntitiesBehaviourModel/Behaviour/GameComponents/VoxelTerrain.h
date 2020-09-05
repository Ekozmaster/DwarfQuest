#pragma once

#include <src/EntitiesBehaviourModel/Behaviour/Behaviour.h>
#include <cstdint>
#include <src/Graphics/OpenGL/Mesh.h>
#include <src/Graphics/OpenGL/Material.h>
#include <glm/glm.hpp>

namespace DwarfQuest {
    namespace GameComponents {


#define CHUNK_WIDTH 16
#define CHUNK_HEIGHT 256

        typedef struct {
            uint8_t id;
        } Block;

        class Chunk {
        public:
            glm::ivec2 id;
            Block* blocks = nullptr;
            Core::Mesh* mesh = nullptr;

            Chunk(glm::ivec2 id);
            ~Chunk();
        } ;

        class VoxelTerrain : public DwarfQuest::Core::Behaviour {
        private:
            int m_renderDistance = 4;
            Chunk** m_chunks;
            Core::Material* material;

            void InitializeChunks();

        public:
            void Init() override;
            void Update() override;
            void Render() override;
            void Destroy() override;
        };
    }
}