#pragma once

#include <src/EntitiesBehaviourModel/Behaviour/Behaviour.h>
#include <cstdint>
#include <src/Graphics/OpenGL/Mesh.h>
#include <src/Graphics/OpenGL/Material.h>
#include <glm/glm.hpp>
#include <src/EntitiesBehaviourModel/Behaviour/GameComponents/VoxelMeshGenerator.h>
#include <src/EntitiesBehaviourModel/Behaviour/GameComponents/BlocksDefinitions.h>

namespace DwarfQuest {
    namespace GameComponents {

        class VoxelTerrain : public DwarfQuest::Core::Behaviour {
        private:
            int foo = 0;
            int m_renderDistance = 16;
            Chunk** m_chunks;
            Core::Material* m_material;
            // x and y encodes the x and z position/id of a chunk, z encodes it's index whithin the m_chunks matrix.
            std::vector<glm::ivec3> m_renderingSpiral;
            std::vector<BlockTexturesInfo> blocksTexturesInfos;
            glm::ivec2 m_pivotChunk = glm::ivec2(0);
            std::vector<Core::Vertex> m_vertices;
            std::vector<unsigned int> m_triangles;

            void GenerateRenderingSpiral();
            void GenerateTerrainTexture();
            void GenerateChunkBlocks(Chunk* chunk, int chunkIndex);
            void GenerateChunkMesh(Chunk* chunk, int chunkIndex);
            void TriggerChunkMatrixTranslation(const glm::ivec2& delta);
            inline Block* QueryNeighbourChunkBlockFromLocalCoords(glm::ivec3 coord, int chunkIndex);

        public:

            void SetPivotChunk(const glm::ivec2& newPivot);

            void Init() override;
            void Update() override;
            void Render() override;
            void Destroy() override;
        };
    }
}