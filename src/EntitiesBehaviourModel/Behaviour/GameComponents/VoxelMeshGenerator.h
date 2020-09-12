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
        public:
            bool blocksInitialized = false;
            bool meshInitialized = false;
            glm::ivec2 id;
            Block* blocks = nullptr;
            Core::Mesh* mesh = nullptr;
            unsigned int threadsRefCount = 0;

            Chunk(glm::ivec2 id);
            ~Chunk();
        };

        class ChunkMeshGeneratorThread {
            std::vector<Core::Vertex> m_vertices;
            std::vector<unsigned int> m_triangles;
            std::thread m_thread;
            std::mutex m_mutexLock;
            Chunk** m_chunks;
            Core::Mesh* m_mesh = nullptr;

        public:
            ChunkMeshGeneratorThread();
            ~ChunkMeshGeneratorThread();

            void InitChunk(Chunk** chunks);
            void Start();
            bool IsRunning();
            Core::Mesh* GetFinalMesh();

        private:
            Block* GetBlock(glm::ivec3 coord);
            void Generate();
        };
    }
}