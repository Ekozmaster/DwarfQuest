#include <chrono>
#include <src/EntitiesBehaviourModel/Behaviour/GameComponents/VoxelTerrain.h>
#include <src/EntitiesBehaviourModel/GameObject.h>
#include <src/ResourceManagement/ResourceManager.h>
#include <vector>
#include <src/Graphics/OpenGL/ShadersDefinitions.h>
#include <glm/gtc/type_ptr.hpp>
#include <src/Graphics/OpenGL/Graphics.h>
#include <glm/glm.hpp>
#include <src/Utils/Logger.h>

namespace DwarfQuest {
    namespace GameComponents {

#define BLOCK_COORD_TO_INDEX(vec) vec.x + CHUNK_WIDTH*(vec.z + vec.y * CHUNK_WIDTH)
#define INDEX_TO_BLOCK_COORD(index) glm::ivec3(index % CHUNK_WIDTH, index / (CHUNK_WIDTH * CHUNK_WIDTH), (index % CHUNK_HEIGHT) / CHUNK_WIDTH)
#define BLOCK_INDEX_UP(index) index + CHUNK_WIDTH * CHUNK_WIDTH
#define BLOCK_INDEX_DOWN(index) index - CHUNK_WIDTH * CHUNK_WIDTH
#define BLOCK_INDEX_WEST(index) index + 1
#define BLOCK_INDEX_EAST(index) index - 1
#define BLOCK_INDEX_NORTH(index) index + CHUNK_WIDTH
#define BLOCK_INDEX_SOUTH(index) index - CHUNK_WIDTH

#define FOR_BLOCK_X() for(int x=0; x < CHUNK_WIDTH; x++, index++)
#define FOR_BLOCK_Y() for(int y=0, index=0; y < CHUNK_HEIGHT; y++)
#define FOR_BLOCK_Z() for(int z=0; z < CHUNK_WIDTH; z++)
        // Iterates correctly in the layout the blocks are allocated in memory to increase cache hits.
#define FOR_BLOCK() FOR_BLOCK_Y() FOR_BLOCK_Z() FOR_BLOCK_X()


#define CHUNK_INDEX_NORTH(index) index + 1
#define CHUNK_INDEX_SOUTH(index) index - 1
#define CHUNK_INDEX_WEST(index) index + m_renderDistance * 2
#define CHUNK_INDEX_EAST(index) index - m_renderDistance * 2
#define CHUNK_INDEX_NORTH_WEST(index) index + 1 + m_renderDistance * 2
#define CHUNK_INDEX_NORTH_EAST(index) index + 1 - m_renderDistance * 2
#define CHUNK_INDEX_SOUTH_WEST(index) index - 1 + m_renderDistance * 2
#define CHUNK_INDEX_SOUTH_EAST(index) index - 1 - m_renderDistance * 2

        Chunk::Chunk(glm::ivec2 id) : id(id) {
            blocks = new Block[CHUNK_WIDTH * CHUNK_WIDTH * CHUNK_HEIGHT];
        }

        Chunk::~Chunk() {
            if (blocks != nullptr) delete[] blocks;
            if (mesh != nullptr) delete mesh;
        }

        void VoxelTerrain::GenerateRenderingSpiral() {
            m_renderingSpiral.clear();
            int chunkCount = m_renderDistance * m_renderDistance * 4;
            m_renderingSpiral.reserve(chunkCount);
            
            for (int x = -m_renderDistance; x < m_renderDistance; x++) {
                for (int z = -m_renderDistance; z < m_renderDistance; z++) {
                    int chunkIndex = z + m_renderDistance + (x + m_renderDistance) * m_renderDistance * 2;
                    glm::ivec3 chunkID(x, z, chunkIndex);
                    float length2CKID = glm::length2(glm::vec2(chunkID.x, chunkID.y));

                    auto it = m_renderingSpiral.begin();
                    while (it != m_renderingSpiral.end() && length2CKID > glm::length2(glm::vec2(it->x, it->y))) {
                        ++it;
                    }

                    if (it != m_renderingSpiral.end()) m_renderingSpiral.emplace(it, chunkID);
                    else m_renderingSpiral.push_back(chunkID);
                }
            }
        }

        void VoxelTerrain::GenerateChunkBlocks(Chunk* chunk, int chunkIndex) {
            glm::vec3 chunkPos = glm::vec3(chunk->id.x, 0, chunk->id.y) * (float)CHUNK_WIDTH;
            glm::vec3 position;
            FOR_BLOCK() {
                position = glm::vec3(x, y, z) + chunkPos;
                if (position.y > (glm::sin(position.x * 0.2f) + glm::sin(position.z * 0.2f)) * 5 + 10)
                    chunk->blocks[index].id = 0;
                else chunk->blocks[index].id = 1;
            }

            chunk->blocksInitialized = true;
        }

        void VoxelTerrain::GenerateChunkMesh(Chunk* chunk, int chunkIndex) {
            int trisIndex = 0;
            std::vector<Core::Vertex> vertices;
            vertices.reserve(3000);
            std::vector<unsigned int> triangles;
            triangles.reserve(4500);

            FOR_BLOCK() {
                glm::ivec3 position(x, y, z);
                if (chunk->blocks[index].id) {
                    if ((x != 0 && chunk->blocks[BLOCK_INDEX_EAST(index)].id == 0) ||
                        (x == 0 && m_chunks[CHUNK_INDEX_EAST(chunkIndex)]->blocks[BLOCK_COORD_TO_INDEX(glm::ivec3(CHUNK_WIDTH - 1, y, z))].id == 0)) { // EAST
                        vertices.push_back(Core::Vertex(glm::vec3(position.x - 0.5f, position.y - 0.5f, position.z - 0.5f), glm::vec3(-1.0, 0.0, 0.0), glm::vec2(0.0, 0.0)));
                        vertices.push_back(Core::Vertex(glm::vec3(position.x - 0.5f, position.y + 0.5f, position.z - 0.5f), glm::vec3(-1.0, 0.0, 0.0), glm::vec2(0.0, 1.0)));
                        vertices.push_back(Core::Vertex(glm::vec3(position.x - 0.5f, position.y + 0.5f, position.z + 0.5f), glm::vec3(-1.0, 0.0, 0.0), glm::vec2(1.0, 1.0)));
                        vertices.push_back(Core::Vertex(glm::vec3(position.x - 0.5f, position.y - 0.5f, position.z + 0.5f), glm::vec3(-1.0, 0.0, 0.0), glm::vec2(1.0, 0.0)));
                        triangles.push_back(trisIndex);
                        triangles.push_back(trisIndex + 1);
                        triangles.push_back(trisIndex + 2);
                        triangles.push_back(trisIndex);
                        triangles.push_back(trisIndex + 2);
                        triangles.push_back(trisIndex + 3);
                        trisIndex += 4;
                    }
                    if ((x != CHUNK_WIDTH - 1 && chunk->blocks[BLOCK_INDEX_WEST(index)].id == 0) ||
                        (x == CHUNK_WIDTH - 1 && m_chunks[CHUNK_INDEX_WEST(chunkIndex)]->blocks[BLOCK_COORD_TO_INDEX(glm::ivec3(0, y, z))].id == 0)) { // WEST
                        vertices.push_back(Core::Vertex(glm::vec3(position.x + 0.5f, position.y - 0.5f, position.z + 0.5f), glm::vec3(1.0, 0.0, 0.0), glm::vec2(0.0, 0.0)));
                        vertices.push_back(Core::Vertex(glm::vec3(position.x + 0.5f, position.y + 0.5f, position.z + 0.5f), glm::vec3(1.0, 0.0, 0.0), glm::vec2(0.0, 1.0)));
                        vertices.push_back(Core::Vertex(glm::vec3(position.x + 0.5f, position.y + 0.5f, position.z - 0.5f), glm::vec3(1.0, 0.0, 0.0), glm::vec2(1.0, 1.0)));
                        vertices.push_back(Core::Vertex(glm::vec3(position.x + 0.5f, position.y - 0.5f, position.z - 0.5f), glm::vec3(1.0, 0.0, 0.0), glm::vec2(1.0, 0.0)));
                        triangles.push_back(trisIndex);
                        triangles.push_back(trisIndex + 1);
                        triangles.push_back(trisIndex + 2);
                        triangles.push_back(trisIndex);
                        triangles.push_back(trisIndex + 2);
                        triangles.push_back(trisIndex + 3);
                        trisIndex += 4;
                    }
                    if ((z != 0 && chunk->blocks[BLOCK_INDEX_SOUTH(index)].id == 0) ||
                        (z == 0 && m_chunks[CHUNK_INDEX_SOUTH(chunkIndex)]->blocks[BLOCK_COORD_TO_INDEX(glm::ivec3(x, y, CHUNK_WIDTH - 1))].id == 0)) { // SOUTH
                        vertices.push_back(Core::Vertex(glm::vec3(position.x + 0.5f, position.y - 0.5f, position.z - 0.5f), glm::vec3(0.0, 0.0, -1.0), glm::vec2(0.0, 0.0)));
                        vertices.push_back(Core::Vertex(glm::vec3(position.x + 0.5f, position.y + 0.5f, position.z - 0.5f), glm::vec3(0.0, 0.0, -1.0), glm::vec2(0.0, 1.0)));
                        vertices.push_back(Core::Vertex(glm::vec3(position.x - 0.5f, position.y + 0.5f, position.z - 0.5f), glm::vec3(0.0, 0.0, -1.0), glm::vec2(1.0, 1.0)));
                        vertices.push_back(Core::Vertex(glm::vec3(position.x - 0.5f, position.y - 0.5f, position.z - 0.5f), glm::vec3(0.0, 0.0, -1.0), glm::vec2(1.0, 0.0)));
                        triangles.push_back(trisIndex);
                        triangles.push_back(trisIndex + 1);
                        triangles.push_back(trisIndex + 2);
                        triangles.push_back(trisIndex);
                        triangles.push_back(trisIndex + 2);
                        triangles.push_back(trisIndex + 3);
                        trisIndex += 4;
                    }
                    if ((z != CHUNK_WIDTH - 1 && chunk->blocks[BLOCK_INDEX_NORTH(index)].id == 0) ||
                        (z == CHUNK_WIDTH - 1 && m_chunks[CHUNK_INDEX_NORTH(chunkIndex)]->blocks[BLOCK_COORD_TO_INDEX(glm::ivec3(x, y, 0))].id == 0)) { // NORTH
                        vertices.push_back(Core::Vertex(glm::vec3(position.x - 0.5f, position.y - 0.5f, position.z + 0.5f), glm::vec3(0.0, 0.0, 1.0), glm::vec2(0.0, 0.0)));
                        vertices.push_back(Core::Vertex(glm::vec3(position.x - 0.5f, position.y + 0.5f, position.z + 0.5f), glm::vec3(0.0, 0.0, 1.0), glm::vec2(0.0, 1.0)));
                        vertices.push_back(Core::Vertex(glm::vec3(position.x + 0.5f, position.y + 0.5f, position.z + 0.5f), glm::vec3(0.0, 0.0, 1.0), glm::vec2(1.0, 1.0)));
                        vertices.push_back(Core::Vertex(glm::vec3(position.x + 0.5f, position.y - 0.5f, position.z + 0.5f), glm::vec3(0.0, 0.0, 1.0), glm::vec2(1.0, 0.0)));
                        triangles.push_back(trisIndex);
                        triangles.push_back(trisIndex + 1);
                        triangles.push_back(trisIndex + 2);
                        triangles.push_back(trisIndex);
                        triangles.push_back(trisIndex + 2);
                        triangles.push_back(trisIndex + 3);
                        trisIndex += 4;
                    }
                    if (y == 0 || chunk->blocks[BLOCK_INDEX_DOWN(index)].id == 0) { // DOWN
                        vertices.push_back(Core::Vertex(glm::vec3(position.x - 0.5f, position.y - 0.5f, position.z - 0.5f), glm::vec3(0.0, -1.0, 0.0), glm::vec2(0.0, 0.0)));
                        vertices.push_back(Core::Vertex(glm::vec3(position.x - 0.5f, position.y - 0.5f, position.z + 0.5f), glm::vec3(0.0, -1.0, 0.0), glm::vec2(0.0, 1.0)));
                        vertices.push_back(Core::Vertex(glm::vec3(position.x + 0.5f, position.y - 0.5f, position.z + 0.5f), glm::vec3(0.0, -1.0, 0.0), glm::vec2(1.0, 1.0)));
                        vertices.push_back(Core::Vertex(glm::vec3(position.x + 0.5f, position.y - 0.5f, position.z - 0.5f), glm::vec3(0.0, -1.0, 0.0), glm::vec2(1.0, 0.0)));
                        triangles.push_back(trisIndex);
                        triangles.push_back(trisIndex + 1);
                        triangles.push_back(trisIndex + 2);
                        triangles.push_back(trisIndex);
                        triangles.push_back(trisIndex + 2);
                        triangles.push_back(trisIndex + 3);
                        trisIndex += 4;
                    }
                    if (y == CHUNK_HEIGHT - 1 || chunk->blocks[BLOCK_INDEX_UP(index)].id == 0) { // UP
                        vertices.push_back(Core::Vertex(glm::vec3(position.x + 0.5f, position.y + 0.5f, position.z - 0.5f), glm::vec3(0.0, 1.0, 0.0), glm::vec2(0.0, 0.0)));
                        vertices.push_back(Core::Vertex(glm::vec3(position.x + 0.5f, position.y + 0.5f, position.z + 0.5f), glm::vec3(0.0, 1.0, 0.0), glm::vec2(0.0, 1.0)));
                        vertices.push_back(Core::Vertex(glm::vec3(position.x - 0.5f, position.y + 0.5f, position.z + 0.5f), glm::vec3(0.0, 1.0, 0.0), glm::vec2(1.0, 1.0)));
                        vertices.push_back(Core::Vertex(glm::vec3(position.x - 0.5f, position.y + 0.5f, position.z - 0.5f), glm::vec3(0.0, 1.0, 0.0), glm::vec2(1.0, 0.0)));
                        triangles.push_back(trisIndex);
                        triangles.push_back(trisIndex + 1);
                        triangles.push_back(trisIndex + 2);
                        triangles.push_back(trisIndex);
                        triangles.push_back(trisIndex + 2);
                        triangles.push_back(trisIndex + 3);
                        trisIndex += 4;
                    }
                }
            }
            if (vertices.size() > 0) {
                chunk->mesh = new Core::Mesh();
                chunk->mesh->Create((GLfloat*)&vertices[0], vertices.size(), (GLuint*)&triangles[0], triangles.size());
                vertices.clear();
                triangles.clear();
            }

            chunk->meshInitialized = true;
        }

        void VoxelTerrain::TriggerChunkMatrixTranslation(const glm::ivec2& delta) {
            int chunksCount = m_renderDistance * m_renderDistance * 4;
            Chunk** newChunks = new Chunk * [chunksCount];
            for (int i = 0; i < chunksCount; i++) newChunks[i] = nullptr;

            for (int x = 0; x < m_renderDistance * 2; x++) {
                for (int z = 0; z < m_renderDistance * 2; z++) {
                    int chunkIndex = z + x * m_renderDistance * 2;
                    glm::ivec2 newPos = glm::ivec2(x, z) - delta;

                    // If out of rendering range.
                    if (newPos.x < 0 || newPos.x >= m_renderDistance * 2 ||
                        newPos.y < 0 || newPos.y >= m_renderDistance * 2) {
                        delete m_chunks[chunkIndex];
                    } else {
                        int newChunkIndex = newPos.y + newPos.x * m_renderDistance * 2;
                        newChunks[newChunkIndex] = m_chunks[chunkIndex];
                    }
                }
            }

            delete m_chunks;
            m_chunks = newChunks;

            // Creating new chunks on the new matrix.
            for (int x = 0; x < m_renderDistance * 2; x++) {
                for (int z = 0; z < m_renderDistance * 2; z++) {
                    int chunkIndex = z + x * m_renderDistance * 2;
                    if (m_chunks[chunkIndex] == nullptr) {
                        m_chunks[chunkIndex] = new Chunk(glm::ivec2(x, z) + m_pivotChunk - m_renderDistance);
                    }
                }
            }
        }

        void VoxelTerrain::SetPivotChunk(const glm::ivec2& newPivot) {
            if (newPivot != m_pivotChunk) {
                glm::ivec2 delta = newPivot - m_pivotChunk;
                m_pivotChunk = newPivot;
                TriggerChunkMatrixTranslation(delta);
            }
        }

        void VoxelTerrain::Init() {
            GenerateRenderingSpiral();
            m_chunks = new Chunk * [m_renderDistance * m_renderDistance * 4];

            int chunkIndex = 0;
            for (int chunkX = -m_renderDistance; chunkX < m_renderDistance; chunkX++) {
                for (int chunkZ = -m_renderDistance; chunkZ < m_renderDistance; chunkZ++) {
                    m_chunks[chunkIndex] = new Chunk(glm::ivec2(chunkX, chunkZ) + m_pivotChunk);
                    chunkIndex++;
                }
            }
            m_material = Core::ResourceManager::GetOrLoadMaterialAsset("Assets/Materials/TerrainMaterial.mat");
        }

        void VoxelTerrain::Update() {
            double totalFrameTime = 0;
            auto startTime = std::chrono::high_resolution_clock::now();

            for (auto it = m_renderingSpiral.begin(); it != m_renderingSpiral.end() && totalFrameTime < 0.16; ++it) {
                if (!m_chunks[it->z]->blocksInitialized) {
                    GenerateChunkBlocks(m_chunks[it->z], it->z);
                } else if (!m_chunks[it->z]->meshInitialized) {
                    // Cheking if all chunks in a neighbourhood of 3x3 have their blocks already initialized,
                    // so that the mesh generation algorithm don't have to worry about that for each block.
                    if (it->x != m_renderDistance - 1 && m_chunks[CHUNK_INDEX_WEST(it->z)]->blocksInitialized &&
                        it->x != -m_renderDistance && m_chunks[CHUNK_INDEX_EAST(it->z)]->blocksInitialized &&
                        it->y != m_renderDistance - 1 && m_chunks[CHUNK_INDEX_NORTH(it->z)]->blocksInitialized &&
                        it->y != -m_renderDistance && m_chunks[CHUNK_INDEX_SOUTH(it->z)]->blocksInitialized &&
                        m_chunks[CHUNK_INDEX_NORTH_WEST(it->z)]->blocksInitialized &&
                        m_chunks[CHUNK_INDEX_NORTH_EAST(it->z)]->blocksInitialized &&
                        m_chunks[CHUNK_INDEX_SOUTH_WEST(it->z)]->blocksInitialized &&
                        m_chunks[CHUNK_INDEX_SOUTH_EAST(it->z)]->blocksInitialized) {
                        GenerateChunkMesh(m_chunks[it->z], it->z);
                    }
                }

                auto endTime = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> diff = endTime - startTime;
                totalFrameTime += diff.count();
            }
        }

        void VoxelTerrain::Render() {
            m_material->Use();
            glm::mat4 model = ((Core::GameObject*)gameObject)->transform.GetTRSMatrix();
            

            int chunkCount = m_renderDistance * m_renderDistance * 4;
            for (int chunkIndex = 0; chunkIndex < chunkCount; chunkIndex++) {
                if (m_chunks[chunkIndex]->mesh != nullptr) {
                    Core::Graphics::SetMesh(m_chunks[chunkIndex]->mesh);
                    glm::vec3 chunkPos = glm::vec3(m_chunks[chunkIndex]->id.x, 0, m_chunks[chunkIndex]->id.y) * (float)CHUNK_WIDTH;
                    glm::mat4 chunkMat = glm::translate(model, chunkPos);
                    Core::Graphics::SetShaderMatrix(SHADERS_MODEL_MATRIX, glm::value_ptr(chunkMat));
                    Core::Graphics::RenderMesh();
                }
            }
        }

        void VoxelTerrain::Destroy() {
            int chunkCount = m_renderDistance * m_renderDistance * 4;
            for (int chunkIndex = 0; chunkIndex < chunkCount; chunkIndex++) delete m_chunks[chunkIndex];
            delete m_chunks;
        }
    }
}