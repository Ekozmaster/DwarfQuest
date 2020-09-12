#include <src/EntitiesBehaviourModel/Behaviour/GameComponents/VoxelMeshGenerator.h>

namespace DwarfQuest {
    namespace GameComponents {

#define BLOCK_COORD_TO_INDEX(vec) vec.x + CHUNK_WIDTH*(vec.z + vec.y * CHUNK_WIDTH)
#define INDEX_TO_BLOCK_COORD(index) glm::ivec3(index % CHUNK_WIDTH, index / (CHUNK_WIDTH * CHUNK_WIDTH), (index % CHUNK_HEIGHT) / CHUNK_WIDTH)
#define BLOCK_INDEX_UP(index) (index) + CHUNK_WIDTH * CHUNK_WIDTH
#define BLOCK_INDEX_DOWN(index) (index) - CHUNK_WIDTH * CHUNK_WIDTH
#define BLOCK_INDEX_WEST(index) (index) - 1
#define BLOCK_INDEX_EAST(index) (index) + 1
#define BLOCK_INDEX_NORTH(index) (index) + CHUNK_WIDTH
#define BLOCK_INDEX_SOUTH(index) (index) - CHUNK_WIDTH

#define CHUNK_INDEX_NORTH 5
#define CHUNK_INDEX_SOUTH 3
#define CHUNK_INDEX_WEST 1
#define CHUNK_INDEX_EAST 7
#define CHUNK_INDEX_NORTH_WEST 2
#define CHUNK_INDEX_NORTH_EAST 8
#define CHUNK_INDEX_SOUTH_WEST 0
#define CHUNK_INDEX_SOUTH_EAST 6

#define FOR_BLOCK_X() for(int x=0; x < CHUNK_WIDTH; x++, index++)
#define FOR_BLOCK_Y() for(int y=0, index=0; y < CHUNK_HEIGHT; y++)
#define FOR_BLOCK_Z() for(int z=0; z < CHUNK_WIDTH; z++)
        // Iterates correctly in the layout the blocks are allocated in memory to increase cache hits.
#define FOR_BLOCK() FOR_BLOCK_Y() FOR_BLOCK_Z() FOR_BLOCK_X()

        ChunkMeshGeneratorThread::ChunkMeshGeneratorThread() {
            m_chunks = new Chunk * [9];
            m_vertices.reserve(3000);
            m_triangles.reserve(4500);
        }

        ChunkMeshGeneratorThread::~ChunkMeshGeneratorThread() {
            if (m_thread.joinable()) {
                m_thread.join();
            }
            delete[] m_chunks;
        }

        void ChunkMeshGeneratorThread::InitChunk(Chunk** chunks) {
            m_mesh = nullptr;
            memcpy(m_chunks, chunks, sizeof(Chunk*) * 9);
        }

        void ChunkMeshGeneratorThread::Start() {
            m_thread = std::thread(&ChunkMeshGeneratorThread::Generate, this);
        }

        bool ChunkMeshGeneratorThread::IsRunning() {
            if (m_mutexLock.try_lock()) {
                m_mutexLock.unlock();
                return false;
            }
            return true;
        }

        Core::Mesh* ChunkMeshGeneratorThread::GetFinalMesh() {
            if (!IsRunning() && m_mesh != nullptr) {
                return m_mesh;
            }
        }

        Block* ChunkMeshGeneratorThread::GetBlock(glm::ivec3 coord) {
            int chunkIndex = 4;
            if (coord.y < 0 || coord.y >= CHUNK_HEIGHT) return nullptr;
            if (coord.x < 0) {
                chunkIndex = CHUNK_INDEX_WEST;
                coord.x = CHUNK_WIDTH + coord.x;
            } else if (coord.x >= CHUNK_WIDTH) {
                chunkIndex = CHUNK_INDEX_EAST;
                coord.x = coord.x - CHUNK_WIDTH;
            }
            if (coord.z < 0) {
                chunkIndex = CHUNK_INDEX_SOUTH;
                coord.z = CHUNK_WIDTH + coord.z;
            } else if (coord.z >= CHUNK_WIDTH) {
                chunkIndex = CHUNK_INDEX_NORTH;
                coord.z = coord.z - CHUNK_WIDTH;
            }
            return &(m_chunks[chunkIndex]->blocks[BLOCK_COORD_TO_INDEX(coord)]);
        }

        void ChunkMeshGeneratorThread::Generate() {
            m_mutexLock.lock();
            m_vertices.clear();
            m_triangles.clear();

            int trisIndex = 0;

            Block* neighBlocks[9];
            glm::vec2 uvBR(1.0f, 0.0f);
            glm::vec2 uvTR(1.0f, 1.0f);
            glm::vec2 uvTL(0.0f, 1.0f);
            glm::vec2 uvBL(0.0f, 0.0f);

            Chunk* chunk = m_chunks[4];

            FOR_BLOCK() {
                glm::ivec3 position(x, y, z); // For integer usage.
                glm::vec3 pos(x, y, z); // For floats usage.
                if (chunk->blocks[index].id) {
                    // EAST
                    if ((x != CHUNK_WIDTH - 1 && chunk->blocks[BLOCK_INDEX_EAST(index)].id == 0) ||
                        (x == CHUNK_WIDTH - 1 && m_chunks[CHUNK_INDEX_EAST]->blocks[BLOCK_COORD_TO_INDEX(glm::ivec3(0, y, z))].id == 0)) {
                        glm::vec3 faceNormal(1.0, 0.0, 0.0);

                        neighBlocks[0] = GetBlock(glm::ivec3(x + 1, y + 1, z - 1));
                        neighBlocks[1] = GetBlock(glm::ivec3(x + 1, y + 1, z));
                        neighBlocks[2] = GetBlock(glm::ivec3(x + 1, y + 1, z + 1));
                        neighBlocks[3] = GetBlock(glm::ivec3(x + 1, y, z - 1));
                        neighBlocks[4] = GetBlock(glm::ivec3(x + 1, y, z));
                        neighBlocks[5] = GetBlock(glm::ivec3(x + 1, y, z + 1));
                        neighBlocks[6] = GetBlock(glm::ivec3(x + 1, y - 1, z - 1));
                        neighBlocks[7] = GetBlock(glm::ivec3(x + 1, y - 1, z));
                        neighBlocks[8] = GetBlock(glm::ivec3(x + 1, y - 1, z + 1));

                        // Bottom Right Vertex.
                        int vertexAOLight = 3;
                        if (neighBlocks[5] && neighBlocks[5]->id != 0 && neighBlocks[7] && neighBlocks[7]->id != 0) vertexAOLight = 0;
                        else vertexAOLight = 3 - ((int)(neighBlocks[5] && neighBlocks[5]->id != 0) +
                            (int)(neighBlocks[7] && neighBlocks[7]->id != 0) +
                            (int)(neighBlocks[8] && neighBlocks[8]->id != 0));
                        m_vertices.push_back(Core::Vertex(pos + glm::vec3(0.5f, -0.5f, 0.5f), faceNormal, uvBR, glm::vec3(vertexAOLight / 3.0f)));

                        // Top Right Vertex.
                        vertexAOLight = 3;
                        if (neighBlocks[5] && neighBlocks[5]->id != 0 && neighBlocks[1] && neighBlocks[1]->id != 0) vertexAOLight = 0;
                        else vertexAOLight = 3 - ((int)(neighBlocks[5] && neighBlocks[5]->id != 0) +
                            (int)(neighBlocks[1] && neighBlocks[1]->id != 0) +
                            (int)(neighBlocks[2] && neighBlocks[2]->id != 0));
                        m_vertices.push_back(Core::Vertex(pos + glm::vec3(0.5f, 0.5f, 0.5f), faceNormal, uvTR, glm::vec3(vertexAOLight / 3.0f)));

                        // Top Left Vertex.
                        vertexAOLight = 3;
                        if (neighBlocks[1] && neighBlocks[1]->id != 0 && neighBlocks[3] && neighBlocks[3]->id != 0) vertexAOLight = 0;
                        else vertexAOLight = 3 - ((int)(neighBlocks[1] && neighBlocks[1]->id != 0) +
                            (int)(neighBlocks[3] && neighBlocks[3]->id != 0) +
                            (int)(neighBlocks[0] && neighBlocks[0]->id != 0));
                        m_vertices.push_back(Core::Vertex(pos + glm::vec3(0.5f, 0.5f, -0.5f), faceNormal, uvTL, glm::vec3(vertexAOLight / 3.0f)));

                        // Bottom Left Vertex.
                        vertexAOLight = 3;
                        if (neighBlocks[7] && neighBlocks[7]->id != 0 && neighBlocks[3] && neighBlocks[3]->id != 0) vertexAOLight = 0;
                        else vertexAOLight = 3 - ((int)(neighBlocks[7] && neighBlocks[7]->id != 0) +
                            (int)(neighBlocks[3] && neighBlocks[3]->id != 0) +
                            (int)(neighBlocks[6] && neighBlocks[6]->id != 0));
                        m_vertices.push_back(Core::Vertex(pos + glm::vec3(0.5f, -0.5f, -0.5f), faceNormal, uvBL, glm::vec3(vertexAOLight / 3.0f)));

                        m_triangles.push_back(trisIndex);
                        m_triangles.push_back(trisIndex + 1);
                        m_triangles.push_back(trisIndex + 2);
                        m_triangles.push_back(trisIndex);
                        m_triangles.push_back(trisIndex + 2);
                        m_triangles.push_back(trisIndex + 3);
                        trisIndex += 4;
                    }
                    // WEST
                    if ((x != 0 && chunk->blocks[BLOCK_INDEX_WEST(index)].id == 0) ||
                        (x == 0 && m_chunks[CHUNK_INDEX_WEST]->blocks[BLOCK_COORD_TO_INDEX(glm::ivec3(CHUNK_WIDTH - 1, y, z))].id == 0)) {
                        glm::vec3 faceNormal(-1.0, 0.0, 0.0);

                        neighBlocks[0] = GetBlock(glm::ivec3(x - 1, y + 1, z + 1));
                        neighBlocks[1] = GetBlock(glm::ivec3(x - 1, y + 1, z));
                        neighBlocks[2] = GetBlock(glm::ivec3(x - 1, y + 1, z - 1));
                        neighBlocks[3] = GetBlock(glm::ivec3(x - 1, y, z + 1));
                        neighBlocks[4] = GetBlock(glm::ivec3(x - 1, y, z));
                        neighBlocks[5] = GetBlock(glm::ivec3(x - 1, y, z - 1));
                        neighBlocks[6] = GetBlock(glm::ivec3(x - 1, y - 1, z + 1));
                        neighBlocks[7] = GetBlock(glm::ivec3(x - 1, y - 1, z));
                        neighBlocks[8] = GetBlock(glm::ivec3(x - 1, y - 1, z - 1));

                        // Bottom Right Vertex.
                        int vertexAOLight = 3;
                        if (neighBlocks[5] && neighBlocks[5]->id != 0 && neighBlocks[7] && neighBlocks[7]->id != 0) vertexAOLight = 0;
                        else vertexAOLight = 3 - ((int)(neighBlocks[5] && neighBlocks[5]->id != 0) +
                            (int)(neighBlocks[7] && neighBlocks[7]->id != 0) +
                            (int)(neighBlocks[8] && neighBlocks[8]->id != 0));
                        m_vertices.push_back(Core::Vertex(pos + glm::vec3(-0.5f, -0.5f, -0.5f), faceNormal, uvBR, glm::vec3(vertexAOLight / 3.0f)));

                        // Top Right Vertex.
                        vertexAOLight = 3;
                        if (neighBlocks[5] && neighBlocks[5]->id != 0 && neighBlocks[1] && neighBlocks[1]->id != 0) vertexAOLight = 0;
                        else vertexAOLight = 3 - ((int)(neighBlocks[5] && neighBlocks[5]->id != 0) +
                            (int)(neighBlocks[1] && neighBlocks[1]->id != 0) +
                            (int)(neighBlocks[2] && neighBlocks[2]->id != 0));
                        m_vertices.push_back(Core::Vertex(pos + glm::vec3(-0.5f, 0.5f, -0.5f), faceNormal, uvTR, glm::vec3(vertexAOLight / 3.0f)));

                        // Top Left Vertex.
                        vertexAOLight = 3;
                        if (neighBlocks[1] && neighBlocks[1]->id != 0 && neighBlocks[3] && neighBlocks[3]->id != 0) vertexAOLight = 0;
                        else vertexAOLight = 3 - ((int)(neighBlocks[1] && neighBlocks[1]->id != 0) +
                            (int)(neighBlocks[3] && neighBlocks[3]->id != 0) +
                            (int)(neighBlocks[0] && neighBlocks[0]->id != 0));
                        m_vertices.push_back(Core::Vertex(pos + glm::vec3(-0.5f, 0.5f, 0.5f), faceNormal, uvTL, glm::vec3(vertexAOLight / 3.0f)));

                        // Bottom Left Vertex.
                        vertexAOLight = 3;
                        if (neighBlocks[7] && neighBlocks[7]->id != 0 && neighBlocks[3] && neighBlocks[3]->id != 0) vertexAOLight = 0;
                        else vertexAOLight = 3 - ((int)(neighBlocks[7] && neighBlocks[7]->id != 0) +
                            (int)(neighBlocks[3] && neighBlocks[3]->id != 0) +
                            (int)(neighBlocks[6] && neighBlocks[6]->id != 0));
                        m_vertices.push_back(Core::Vertex(pos + glm::vec3(-0.5f, -0.5f, 0.5f), faceNormal, uvBL, glm::vec3(vertexAOLight / 3.0f)));

                        m_triangles.push_back(trisIndex);
                        m_triangles.push_back(trisIndex + 1);
                        m_triangles.push_back(trisIndex + 2);
                        m_triangles.push_back(trisIndex);
                        m_triangles.push_back(trisIndex + 2);
                        m_triangles.push_back(trisIndex + 3);
                        trisIndex += 4;
                    }
                    // SOUTH
                    if ((z != 0 && chunk->blocks[BLOCK_INDEX_SOUTH(index)].id == 0) ||
                        (z == 0 && m_chunks[CHUNK_INDEX_SOUTH]->blocks[BLOCK_COORD_TO_INDEX(glm::ivec3(x, y, CHUNK_WIDTH - 1))].id == 0)) {
                        glm::vec3 faceNormal(0.0, 0.0, -1.0);

                        neighBlocks[0] = GetBlock(glm::ivec3(x - 1, y + 1, z - 1));
                        neighBlocks[1] = GetBlock(glm::ivec3(x, y + 1, z - 1));
                        neighBlocks[2] = GetBlock(glm::ivec3(x + 1, y + 1, z - 1));
                        neighBlocks[3] = GetBlock(glm::ivec3(x - 1, y, z - 1));
                        neighBlocks[4] = GetBlock(glm::ivec3(x, y, z - 1));
                        neighBlocks[5] = GetBlock(glm::ivec3(x + 1, y, z - 1));
                        neighBlocks[6] = GetBlock(glm::ivec3(x - 1, y - 1, z - 1));
                        neighBlocks[7] = GetBlock(glm::ivec3(x, y - 1, z - 1));
                        neighBlocks[8] = GetBlock(glm::ivec3(x + 1, y - 1, z - 1));

                        // Bottom Right Vertex.
                        int vertexAOLight = 3;
                        if (neighBlocks[5] && neighBlocks[5]->id != 0 && neighBlocks[7] && neighBlocks[7]->id != 0) vertexAOLight = 0;
                        else vertexAOLight = 3 - ((int)(neighBlocks[5] && neighBlocks[5]->id != 0) +
                            (int)(neighBlocks[7] && neighBlocks[7]->id != 0) +
                            (int)(neighBlocks[8] && neighBlocks[8]->id != 0));
                        m_vertices.push_back(Core::Vertex(pos + glm::vec3(0.5f, -0.5f, -0.5f), faceNormal, uvBR, glm::vec3(vertexAOLight / 3.0f)));

                        // Top Right Vertex.
                        vertexAOLight = 3;
                        if (neighBlocks[5] && neighBlocks[5]->id != 0 && neighBlocks[1] && neighBlocks[1]->id != 0) vertexAOLight = 0;
                        else vertexAOLight = 3 - ((int)(neighBlocks[5] && neighBlocks[5]->id != 0) +
                            (int)(neighBlocks[1] && neighBlocks[1]->id != 0) +
                            (int)(neighBlocks[2] && neighBlocks[2]->id != 0));
                        m_vertices.push_back(Core::Vertex(pos + glm::vec3(0.5f, 0.5f, -0.5f), faceNormal, uvTR, glm::vec3(vertexAOLight / 3.0f)));

                        // Top Left Vertex.
                        vertexAOLight = 3;
                        if (neighBlocks[1] && neighBlocks[1]->id != 0 && neighBlocks[3] && neighBlocks[3]->id != 0) vertexAOLight = 0;
                        else vertexAOLight = 3 - ((int)(neighBlocks[1] && neighBlocks[1]->id != 0) +
                            (int)(neighBlocks[3] && neighBlocks[3]->id != 0) +
                            (int)(neighBlocks[0] && neighBlocks[0]->id != 0));
                        m_vertices.push_back(Core::Vertex(pos + glm::vec3(-0.5f, 0.5f, -0.5f), faceNormal, uvTL, glm::vec3(vertexAOLight / 3.0f)));

                        // Bottom Left Vertex.
                        vertexAOLight = 3;
                        if (neighBlocks[7] && neighBlocks[7]->id != 0 && neighBlocks[3] && neighBlocks[3]->id != 0) vertexAOLight = 0;
                        else vertexAOLight = 3 - ((int)(neighBlocks[7] && neighBlocks[7]->id != 0) +
                            (int)(neighBlocks[3] && neighBlocks[3]->id != 0) +
                            (int)(neighBlocks[6] && neighBlocks[6]->id != 0));
                        m_vertices.push_back(Core::Vertex(pos + glm::vec3(-0.5f, -0.5f, -0.5f), faceNormal, uvBL, glm::vec3(vertexAOLight / 3.0f)));

                        m_triangles.push_back(trisIndex);
                        m_triangles.push_back(trisIndex + 1);
                        m_triangles.push_back(trisIndex + 2);
                        m_triangles.push_back(trisIndex);
                        m_triangles.push_back(trisIndex + 2);
                        m_triangles.push_back(trisIndex + 3);
                        trisIndex += 4;
                    }
                    // NORTH
                    if ((z != CHUNK_WIDTH - 1 && chunk->blocks[BLOCK_INDEX_NORTH(index)].id == 0) ||
                        (z == CHUNK_WIDTH - 1 && m_chunks[CHUNK_INDEX_NORTH]->blocks[BLOCK_COORD_TO_INDEX(glm::ivec3(x, y, 0))].id == 0)) {
                        glm::vec3 faceNormal(0.0, 0.0, 1.0);

                        neighBlocks[0] = GetBlock(glm::ivec3(x + 1, y + 1, z + 1));
                        neighBlocks[1] = GetBlock(glm::ivec3(x, y + 1, z + 1));
                        neighBlocks[2] = GetBlock(glm::ivec3(x - 1, y + 1, z + 1));
                        neighBlocks[3] = GetBlock(glm::ivec3(x + 1, y, z + 1));
                        neighBlocks[4] = GetBlock(glm::ivec3(x, y, z + 1));
                        neighBlocks[5] = GetBlock(glm::ivec3(x - 1, y, z + 1));
                        neighBlocks[6] = GetBlock(glm::ivec3(x + 1, y - 1, z + 1));
                        neighBlocks[7] = GetBlock(glm::ivec3(x, y - 1, z + 1));
                        neighBlocks[8] = GetBlock(glm::ivec3(x - 1, y - 1, z + 1));

                        // Bottom Right Vertex.
                        int vertexAOLight = 3;
                        if (neighBlocks[5] && neighBlocks[5]->id != 0 && neighBlocks[7] && neighBlocks[7]->id != 0) vertexAOLight = 0;
                        else vertexAOLight = 3 - ((int)(neighBlocks[5] && neighBlocks[5]->id != 0) +
                            (int)(neighBlocks[7] && neighBlocks[7]->id != 0) +
                            (int)(neighBlocks[8] && neighBlocks[8]->id != 0));
                        m_vertices.push_back(Core::Vertex(pos + glm::vec3(-0.5f, -0.5f, 0.5f), faceNormal, uvBR, glm::vec3(vertexAOLight / 3.0f)));

                        // Top Right Vertex.
                        vertexAOLight = 3;
                        if (neighBlocks[5] && neighBlocks[5]->id != 0 && neighBlocks[1] && neighBlocks[1]->id != 0) vertexAOLight = 0;
                        else vertexAOLight = 3 - ((int)(neighBlocks[5] && neighBlocks[5]->id != 0) +
                            (int)(neighBlocks[1] && neighBlocks[1]->id != 0) +
                            (int)(neighBlocks[2] && neighBlocks[2]->id != 0));
                        m_vertices.push_back(Core::Vertex(pos + glm::vec3(-0.5f, 0.5f, 0.5f), faceNormal, uvTR, glm::vec3(vertexAOLight / 3.0f)));

                        // Top Left Vertex.
                        vertexAOLight = 3;
                        if (neighBlocks[1] && neighBlocks[1]->id != 0 && neighBlocks[3] && neighBlocks[3]->id != 0) vertexAOLight = 0;
                        else vertexAOLight = 3 - ((int)(neighBlocks[1] && neighBlocks[1]->id != 0) +
                            (int)(neighBlocks[3] && neighBlocks[3]->id != 0) +
                            (int)(neighBlocks[0] && neighBlocks[0]->id != 0));
                        m_vertices.push_back(Core::Vertex(pos + glm::vec3(0.5f, 0.5f, 0.5f), faceNormal, uvTL, glm::vec3(vertexAOLight / 3.0f)));

                        // Bottom Left Vertex.
                        vertexAOLight = 3;
                        if (neighBlocks[7] && neighBlocks[7]->id != 0 && neighBlocks[3] && neighBlocks[3]->id != 0) vertexAOLight = 0;
                        else vertexAOLight = 3 - ((int)(neighBlocks[7] && neighBlocks[7]->id != 0) +
                            (int)(neighBlocks[3] && neighBlocks[3]->id != 0) +
                            (int)(neighBlocks[6] && neighBlocks[6]->id != 0));
                        m_vertices.push_back(Core::Vertex(pos + glm::vec3(0.5f, -0.5f, 0.5f), faceNormal, uvBL, glm::vec3(vertexAOLight / 3.0f)));

                        m_triangles.push_back(trisIndex);
                        m_triangles.push_back(trisIndex + 1);
                        m_triangles.push_back(trisIndex + 2);
                        m_triangles.push_back(trisIndex);
                        m_triangles.push_back(trisIndex + 2);
                        m_triangles.push_back(trisIndex + 3);
                        trisIndex += 4;
                    }
                    // DOWN
                    if (y == 0 || chunk->blocks[BLOCK_INDEX_DOWN(index)].id == 0) {
                        glm::vec3 faceNormal(0.0, -1.0, 0.0);

                        neighBlocks[0] = GetBlock(glm::ivec3(x + 1, y - 1, z + 1));
                        neighBlocks[1] = GetBlock(glm::ivec3(x, y - 1, z + 1));
                        neighBlocks[2] = GetBlock(glm::ivec3(x - 1, y - 1, z + 1));
                        neighBlocks[3] = GetBlock(glm::ivec3(x + 1, y - 1, z));
                        neighBlocks[4] = GetBlock(glm::ivec3(x, y - 1, z));
                        neighBlocks[5] = GetBlock(glm::ivec3(x - 1, y - 1, z));
                        neighBlocks[6] = GetBlock(glm::ivec3(x + 1, y - 1, z - 1));
                        neighBlocks[7] = GetBlock(glm::ivec3(x, y - 1, z - 1));
                        neighBlocks[8] = GetBlock(glm::ivec3(x - 1, y - 1, z - 1));

                        // Bottom Right Vertex.
                        int vertexAOLight = 3;
                        if (neighBlocks[5] && neighBlocks[5]->id != 0 && neighBlocks[7] && neighBlocks[7]->id != 0) vertexAOLight = 0;
                        else vertexAOLight = 3 - ((int)(neighBlocks[5] && neighBlocks[5]->id != 0) +
                            (int)(neighBlocks[7] && neighBlocks[7]->id != 0) +
                            (int)(neighBlocks[8] && neighBlocks[8]->id != 0));
                        m_vertices.push_back(Core::Vertex(pos + glm::vec3(-0.5f, -0.5f, -0.5f), faceNormal, uvBR, glm::vec3(vertexAOLight / 3.0f)));

                        // Top Right Vertex.
                        vertexAOLight = 3;
                        if (neighBlocks[5] && neighBlocks[5]->id != 0 && neighBlocks[1] && neighBlocks[1]->id != 0) vertexAOLight = 0;
                        else vertexAOLight = 3 - ((int)(neighBlocks[5] && neighBlocks[5]->id != 0) +
                            (int)(neighBlocks[1] && neighBlocks[1]->id != 0) +
                            (int)(neighBlocks[2] && neighBlocks[2]->id != 0));
                        m_vertices.push_back(Core::Vertex(pos + glm::vec3(-0.5f, -0.5f, 0.5f), faceNormal, uvTR, glm::vec3(vertexAOLight / 3.0f)));

                        // Top Left Vertex.
                        vertexAOLight = 3;
                        if (neighBlocks[1] && neighBlocks[1]->id != 0 && neighBlocks[3] && neighBlocks[3]->id != 0) vertexAOLight = 0;
                        else vertexAOLight = 3 - ((int)(neighBlocks[1] && neighBlocks[1]->id != 0) +
                            (int)(neighBlocks[3] && neighBlocks[3]->id != 0) +
                            (int)(neighBlocks[0] && neighBlocks[0]->id != 0));
                        m_vertices.push_back(Core::Vertex(pos + glm::vec3(0.5f, -0.5f, 0.5f), faceNormal, uvTL, glm::vec3(vertexAOLight / 3.0f)));

                        // Bottom Left Vertex.
                        vertexAOLight = 3;
                        if (neighBlocks[7] && neighBlocks[7]->id != 0 && neighBlocks[3] && neighBlocks[3]->id != 0) vertexAOLight = 0;
                        else vertexAOLight = 3 - ((int)(neighBlocks[7] && neighBlocks[7]->id != 0) +
                            (int)(neighBlocks[3] && neighBlocks[3]->id != 0) +
                            (int)(neighBlocks[6] && neighBlocks[6]->id != 0));
                        m_vertices.push_back(Core::Vertex(pos + glm::vec3(0.5f, -0.5f, -0.5f), faceNormal, uvBL, glm::vec3(vertexAOLight / 3.0f)));

                        m_triangles.push_back(trisIndex);
                        m_triangles.push_back(trisIndex + 1);
                        m_triangles.push_back(trisIndex + 2);
                        m_triangles.push_back(trisIndex);
                        m_triangles.push_back(trisIndex + 2);
                        m_triangles.push_back(trisIndex + 3);
                        trisIndex += 4;
                    }
                    // UP
                    if (y == CHUNK_HEIGHT - 1 || chunk->blocks[BLOCK_INDEX_UP(index)].id == 0) {
                        glm::vec3 faceNormal(0.0, 1.0, 0.0);

                        neighBlocks[0] = GetBlock(glm::ivec3(x - 1, y + 1, z + 1));
                        neighBlocks[1] = GetBlock(glm::ivec3(x, y + 1, z + 1));
                        neighBlocks[2] = GetBlock(glm::ivec3(x + 1, y + 1, z + 1));
                        neighBlocks[3] = GetBlock(glm::ivec3(x - 1, y + 1, z));
                        neighBlocks[4] = GetBlock(glm::ivec3(x, y + 1, z));
                        neighBlocks[5] = GetBlock(glm::ivec3(x + 1, y + 1, z));
                        neighBlocks[6] = GetBlock(glm::ivec3(x - 1, y + 1, z - 1));
                        neighBlocks[7] = GetBlock(glm::ivec3(x, y + 1, z - 1));
                        neighBlocks[8] = GetBlock(glm::ivec3(x + 1, y + 1, z - 1));

                        // Bottom Right Vertex.
                        int vertexAOLight = 3;
                        if (neighBlocks[5] && neighBlocks[5]->id != 0 && neighBlocks[7] && neighBlocks[7]->id != 0) vertexAOLight = 0;
                        else vertexAOLight = 3 - ((int)(neighBlocks[5] && neighBlocks[5]->id != 0) +
                            (int)(neighBlocks[7] && neighBlocks[7]->id != 0) +
                            (int)(neighBlocks[8] && neighBlocks[8]->id != 0));
                        m_vertices.push_back(Core::Vertex(pos + glm::vec3(0.5f, 0.5f, -0.5f), faceNormal, uvBR, glm::vec3(vertexAOLight / 3.0f)));

                        // Top Right Vertex.
                        vertexAOLight = 3;
                        if (neighBlocks[5] && neighBlocks[5]->id != 0 && neighBlocks[1] && neighBlocks[1]->id != 0) vertexAOLight = 0;
                        else vertexAOLight = 3 - ((int)(neighBlocks[5] && neighBlocks[5]->id != 0) +
                            (int)(neighBlocks[1] && neighBlocks[1]->id != 0) +
                            (int)(neighBlocks[2] && neighBlocks[2]->id != 0));
                        m_vertices.push_back(Core::Vertex(pos + glm::vec3(0.5f, 0.5f, 0.5f), faceNormal, uvTR, glm::vec3(vertexAOLight / 3.0f)));

                        // Top Left Vertex.
                        vertexAOLight = 3;
                        if (neighBlocks[1] && neighBlocks[1]->id != 0 && neighBlocks[3] && neighBlocks[3]->id != 0) vertexAOLight = 0;
                        else vertexAOLight = 3 - ((int)(neighBlocks[1] && neighBlocks[1]->id != 0) +
                            (int)(neighBlocks[3] && neighBlocks[3]->id != 0) +
                            (int)(neighBlocks[0] && neighBlocks[0]->id != 0));
                        m_vertices.push_back(Core::Vertex(pos + glm::vec3(-0.5f, 0.5f, 0.5f), faceNormal, uvTL, glm::vec3(vertexAOLight / 3.0f)));

                        // Bottom Left Vertex.
                        vertexAOLight = 3;
                        if (neighBlocks[7] && neighBlocks[7]->id != 0 && neighBlocks[3] && neighBlocks[3]->id != 0) vertexAOLight = 0;
                        else vertexAOLight = 3 - ((int)(neighBlocks[7] && neighBlocks[7]->id != 0) +
                            (int)(neighBlocks[3] && neighBlocks[3]->id != 0) +
                            (int)(neighBlocks[6] && neighBlocks[6]->id != 0));
                        m_vertices.push_back(Core::Vertex(pos + glm::vec3(-0.5f, 0.5f, -0.5f), faceNormal, uvBL, glm::vec3(vertexAOLight / 3.0f)));

                        m_triangles.push_back(trisIndex);
                        m_triangles.push_back(trisIndex + 1);
                        m_triangles.push_back(trisIndex + 2);
                        m_triangles.push_back(trisIndex);
                        m_triangles.push_back(trisIndex + 2);
                        m_triangles.push_back(trisIndex + 3);
                        trisIndex += 4;
                    }
                }
            }
            if (m_vertices.size() > 0) {
                m_mesh = new Core::Mesh();
                m_mesh->Create((GLfloat*)&m_vertices[0], m_vertices.size(), (GLuint*)&m_vertices[0], m_triangles.size());
            }

            m_mutexLock.unlock();
        }
    }
}
