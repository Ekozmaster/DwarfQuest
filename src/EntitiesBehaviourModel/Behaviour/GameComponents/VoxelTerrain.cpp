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

#define CHUNK_COORD_TO_INDEX(vec) vec.x + CHUNK_WIDTH*(vec.z + vec.y * CHUNK_WIDTH)
#define INDEX_TO_CHUNK_COORD(index) glm::ivec3(index % CHUNK_WIDTH, index / (CHUNK_WIDTH * CHUNK_WIDTH), (index % CHUNK_HEIGHT) / CHUNK_WIDTH)
#define CHUNK_INDEX_UP(index) index + CHUNK_WIDTH * CHUNK_WIDTH
#define CHUNK_INDEX_DOWN(index) index - CHUNK_WIDTH * CHUNK_WIDTH
#define CHUNK_INDEX_WEST(index) index - 1
#define CHUNK_INDEX_EAST(index) index + 1
#define CHUNK_INDEX_NORTH(index) index + CHUNK_WIDTH
#define CHUNK_INDEX_SOUTH(index) index - CHUNK_WIDTH

#define FOR_CHUNK_X() for(int x=0; x < CHUNK_WIDTH; x++, index++)
#define FOR_CHUNK_Y() for(int y=0, index=0; y < CHUNK_HEIGHT; y++)
#define FOR_CHUNK_Z() for(int z=0; z < CHUNK_WIDTH; z++)
// Iterates correctly in the layout the blocks are allocated in memory to increase cache hits.
#define FOR_CHUNK() FOR_CHUNK_Y() FOR_CHUNK_Z() FOR_CHUNK_X()
        Chunk::Chunk(glm::ivec2 id) : id(id) {
            blocks = new Block[CHUNK_WIDTH * CHUNK_WIDTH * CHUNK_HEIGHT];
        }

        Chunk::~Chunk() {
            if (blocks != nullptr) delete[] blocks;
            if (mesh != nullptr) delete mesh;
        }

        void VoxelTerrain::InitializeChunks() {
            m_chunks = new Chunk * [m_renderDistance * m_renderDistance * 4];

            int chunkIndex = 0;
            std::vector<Core::Vertex> vertices;
            vertices.reserve(3000);
            std::vector<unsigned int> triangles;
            triangles.reserve(4500);
            // Generating a matrix of chunks.
            for (int chunkX = -m_renderDistance; chunkX < m_renderDistance; chunkX++) {
                for (int chunkZ = -m_renderDistance; chunkZ < m_renderDistance; chunkZ++) {
                    m_chunks[chunkIndex] = new Chunk(glm::ivec2(chunkX, chunkZ));
                    
                    glm::vec3 chunkPos = glm::vec3(m_chunks[chunkIndex]->id.x, 0, m_chunks[chunkIndex]->id.y) * (float)CHUNK_WIDTH;
                    glm::vec3 position;

                    FOR_CHUNK() {
                        position = glm::vec3(x, y, z) + chunkPos;
                        if (position.y > (glm::sin(position.x * 0.2f) + glm::sin(position.z * 0.2f)) * 5 + 10)
                            m_chunks[chunkIndex]->blocks[index].id = 0;
                        else m_chunks[chunkIndex]->blocks[index].id = 1;
                    }

                    
                    int trisIndex = 0;

                    FOR_CHUNK() {
                        glm::ivec3 position(x, y, z);
                        if (m_chunks[chunkIndex]->blocks[index].id) {
                            if (x == 0 || m_chunks[chunkIndex]->blocks[CHUNK_INDEX_WEST(index)].id == 0) { // WEST
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
                            if (x == CHUNK_WIDTH - 1 || m_chunks[chunkIndex]->blocks[CHUNK_INDEX_EAST(index)].id == 0) { // EAST
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
                            if (z == 0 || m_chunks[chunkIndex]->blocks[CHUNK_INDEX_SOUTH(index)].id == 0) { // SOUTH
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
                            if (z == CHUNK_WIDTH - 1 || m_chunks[chunkIndex]->blocks[CHUNK_INDEX_NORTH(index)].id == 0) { // NORTH
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
                            if (y == 0 || m_chunks[chunkIndex]->blocks[CHUNK_INDEX_DOWN(index)].id == 0) { // DOWN
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
                            if (y == CHUNK_HEIGHT - 1 || m_chunks[chunkIndex]->blocks[CHUNK_INDEX_UP(index)].id == 0) { // UP
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
                        m_chunks[chunkIndex]->mesh = new Core::Mesh();
                        m_chunks[chunkIndex]->mesh->Create((GLfloat*)&vertices[0], vertices.size(), (GLuint*)&triangles[0], triangles.size());
                        vertices.clear();
                        triangles.clear();
                    }
                    chunkIndex++;
                }
            }
        }

        void VoxelTerrain::Init() {
            auto startTime = std::chrono::high_resolution_clock::now();
            InitializeChunks();
            material = Core::ResourceManager::GetOrLoadMaterialAsset("Assets/Materials/TerrainMaterial.mat");
            auto endTime = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> diff = endTime - startTime;
            double deltaTime = diff.count();
            Core::Logger::Info(std::string("Generated terrain in: " + std::to_string(deltaTime)));
        }

        void VoxelTerrain::Update() {

        }

        void VoxelTerrain::Render() {
            material->Use();
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