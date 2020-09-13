#pragma once

#include <thread>
#include <mutex>
#include <vector>
#include <src/Graphics/OpenGL/Mesh.h>

namespace DwarfQuest {
    namespace GameComponents {

#define CHUNK_WIDTH 16
#define CHUNK_HEIGHT 256

        typedef struct {
            uint8_t id;
        } Block;

        class Chunk {
            Core::Mesh* m_mesh = nullptr;

        public:
            bool blocksInitialized = false;
            bool meshInitialized = false;
            glm::ivec2 id;
            Block* blocks = nullptr;
            unsigned int threadsRefCount = 0;

            Chunk(glm::ivec2 id);
            ~Chunk();

            void SetMesh(Core::Mesh* mesh);
            Core::Mesh* GetMesh();
        };

        class ChunkMeshGeneratorThread {
            std::vector<Core::Vertex> m_vertices;
            std::vector<unsigned int> m_triangles;
            std::thread* m_thread = nullptr;
            std::mutex* m_mutexLock = nullptr;
            Chunk* m_chunks[9];
            bool m_finished = false;

        public:
            ChunkMeshGeneratorThread();
            ~ChunkMeshGeneratorThread();

            void InitChunk(Chunk** chunks);
            void Start();
            void Finish();
            void ResetStatus();
            bool IsRunning();
            bool IsReady();
            void Join();
            Chunk* GetChunk();

        private:
            Block* GetBlock(glm::ivec3 coord);
            void Generate();
        };
    }
}