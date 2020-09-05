#pragma once

#include <src/EntitiesBehaviourModel/Behaviour/Behaviour.h>
#include <cstdint>
#include <src/Graphics/OpenGL/Mesh.h>
#include <src/Graphics/OpenGL/Material.h>

namespace DwarfQuest {
    namespace GameComponents {


#define CHUNK_WIDTH 16
#define CHUNK_HEIGHT 256

        typedef struct {
            uint8_t id;
        } Block;

        class Chunk {
        public:
            Block* blocks = nullptr;
            Core::Mesh* mesh = nullptr;

            Chunk();
            ~Chunk();
        } ;

        class VoxelTerrain : public DwarfQuest::Core::Behaviour {
        private:
            size_t m_renderDistance = 8;
            Chunk* m_chunks;
            Core::Material* material;

        public:
            void Init() override;
            void Update() override;
            void Render() override;
            void Destroy() override;
        };
    }
}