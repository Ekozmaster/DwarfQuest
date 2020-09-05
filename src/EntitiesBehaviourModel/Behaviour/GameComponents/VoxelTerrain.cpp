#include <src/EntitiesBehaviourModel/Behaviour/GameComponents/VoxelTerrain.h>
#include <src/EntitiesBehaviourModel/GameObject.h>
#include <src/ResourceManagement/ResourceManager.h>
#include <vector>
#include <src/Graphics/OpenGL/ShadersDefinitions.h>
#include <glm/gtc/type_ptr.hpp>
#include <src/Graphics/OpenGL/Graphics.h>
#include <glm/glm.hpp>

namespace DwarfQuest {
    namespace GameComponents {

#define CHUNK_COORD_TO_INDEX(vec) vec.x + CHUNK_WIDTH*(vec.z + vec.y * CHUNK_WIDTH)
#define INDEX_TO_CHUNK_COORD(index) glm::ivec3(index % CHUNK_WIDTH, index / (CHUNK_WIDTH * CHUNK_WIDTH), (index % CHUNK_HEIGHT) / CHUNK_WIDTH)

#define FOR_CHUNK_X() for(int x=0; x < CHUNK_WIDTH; x++, index++)
#define FOR_CHUNK_Y() for(int y=0, index=0; y < CHUNK_HEIGHT; y++)
#define FOR_CHUNK_Z() for(int z=0; z < CHUNK_WIDTH; z++)
// Iterates correctly in the layout the blocks are allocated in memory to increase cache hits.
#define FOR_CHUNK() FOR_CHUNK_Y() FOR_CHUNK_Z() FOR_CHUNK_X()
        Chunk::Chunk() {
            blocks = new Block[CHUNK_WIDTH * CHUNK_WIDTH * CHUNK_HEIGHT];
            mesh = new Core::Mesh();
        }

        Chunk::~Chunk() {
            if (blocks != nullptr) delete[] blocks;
            if (mesh != nullptr) delete mesh;
        }

        void VoxelTerrain::Init() {
            m_chunks = new Chunk();
            
            FOR_CHUNK() {
                glm::ivec3 position(x, y, z);
                if (glm::length(glm::vec3(position.x, position.y, position.z)) > 10) m_chunks->blocks[index].id = 0;
                else m_chunks->blocks[index].id = 1;
            }

            std::vector<Core::Vertex> vertices;
            std::vector<unsigned int> triangles;
            int trisIndex = 0;
            FOR_CHUNK() {
                glm::ivec3 position(x, y, z);
                glm::ivec3 up(x, y + 1, z);
                glm::ivec3 down(x, y - 1, z);
                glm::ivec3 east(x + 1, y, z);
                glm::ivec3 north(x, y, z + 1);
                glm::ivec3 south(x, y, z - 1);
                glm::ivec3 west(x - 1, y, z);
                if (m_chunks->blocks[index].id) {
                    if (x == 0 || m_chunks->blocks[CHUNK_COORD_TO_INDEX(west)].id == 0) { // WEST
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
                    if (x == CHUNK_WIDTH - 1 || m_chunks->blocks[CHUNK_COORD_TO_INDEX(east)].id == 0) { // EAST
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
                    if (z == 0 || m_chunks->blocks[CHUNK_COORD_TO_INDEX(south)].id == 0) { // SOUTH
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
                    if (z == CHUNK_WIDTH - 1 || m_chunks->blocks[CHUNK_COORD_TO_INDEX(north)].id == 0) { // NORTH
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
                    if (y == 0 || m_chunks->blocks[CHUNK_COORD_TO_INDEX(down)].id == 0) { // DOWN
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
                    if (y == CHUNK_HEIGHT - 1 || m_chunks->blocks[CHUNK_COORD_TO_INDEX(up)].id == 0) { // UP
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
                m_chunks->mesh->Create((GLfloat*)&vertices[0], vertices.size(), (GLuint*)&triangles[0], triangles.size());
            }
            material = Core::ResourceManager::GetOrLoadMaterialAsset("Assets/Materials/TerrainMaterial.mat");
        }

        void VoxelTerrain::Update() {

        }

        void VoxelTerrain::Render() {
            if (m_chunks->mesh != nullptr) {
                material->Use();
                glm::mat4 model = ((Core::GameObject*)gameObject)->transform.GetTRSMatrix();
                Core::Graphics::SetShaderMatrix(SHADERS_MODEL_MATRIX, glm::value_ptr(model));

                // Rendering.
                Core::Graphics::SetMesh(m_chunks->mesh);
                Core::Graphics::RenderMesh();
            }
        }

        void VoxelTerrain::Destroy() {
            delete m_chunks;
        }
    }
}