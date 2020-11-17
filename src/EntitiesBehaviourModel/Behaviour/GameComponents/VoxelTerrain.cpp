#include <chrono>
#include <stdlib.h>
#include <src/EntitiesBehaviourModel/Behaviour/GameComponents/VoxelTerrain.h>
#include <src/EntitiesBehaviourModel/GameObject.h>
#include <src/ResourceManagement/ResourceManager.h>
#include <vector>
#include <src/Graphics/OpenGL/ShadersDefinitions.h>
#include <glm/gtc/type_ptr.hpp>
#include <src/Graphics/OpenGL/Graphics.h>
#include <glm/glm.hpp>
#include <src/Utils/Logger.h>
#include <iostream>
#include <filesystem>
#include <src/ResourceManagement/ImageLoader.h>

namespace GameComponents {

#define BLOCK_COORD_TO_INDEX(vec) vec.x + CHUNK_WIDTH*(vec.z + vec.y * CHUNK_WIDTH)
#define INDEX_TO_BLOCK_COORD(index) glm::ivec3(index % CHUNK_WIDTH, index / (CHUNK_WIDTH * CHUNK_WIDTH), (index % CHUNK_HEIGHT) / CHUNK_WIDTH)
#define BLOCK_INDEX_UP(index) (index) + CHUNK_WIDTH * CHUNK_WIDTH
#define BLOCK_INDEX_DOWN(index) (index) - CHUNK_WIDTH * CHUNK_WIDTH
#define BLOCK_INDEX_WEST(index) (index) - 1
#define BLOCK_INDEX_EAST(index) (index) + 1
#define BLOCK_INDEX_NORTH(index) (index) + CHUNK_WIDTH
#define BLOCK_INDEX_SOUTH(index) (index) - CHUNK_WIDTH

#define FOR_BLOCK_X() for(int x=0; x < CHUNK_WIDTH; x++, index++)
#define FOR_BLOCK_Y() for(int y=0, index=0; y < CHUNK_HEIGHT; y++)
#define FOR_BLOCK_Z() for(int z=0; z < CHUNK_WIDTH; z++)
    // Iterates correctly in the layout the blocks are allocated in memory to increase cache hits.
#define FOR_BLOCK() FOR_BLOCK_Y() FOR_BLOCK_Z() FOR_BLOCK_X()


#define CHUNK_INDEX_NORTH(index) (index) + 1
#define CHUNK_INDEX_SOUTH(index) (index) - 1
#define CHUNK_INDEX_WEST(index) (index) - m_renderDistance * 2
#define CHUNK_INDEX_EAST(index) (index) + m_renderDistance * 2
#define CHUNK_INDEX_NORTH_WEST(index) (index) + 1 - m_renderDistance * 2
#define CHUNK_INDEX_NORTH_EAST(index) (index) + 1 + m_renderDistance * 2
#define CHUNK_INDEX_SOUTH_WEST(index) (index) - 1 - m_renderDistance * 2
#define CHUNK_INDEX_SOUTH_EAST(index) (index) - 1 + m_renderDistance * 2

    Block* VoxelTerrain::QueryNeighbourChunkBlockFromLocalCoords(glm::ivec3 coord, int chunkIndex) {
        // This method receives a block's position in chunk's local coordinates, which might be 
        // out of the chunk's bounds (eg: x == -1) and finds which chunk it must query blocks from.
        // For example, with coord.x == -3, it will get the block of it's east neightbour chunk, 
        // at position CHUNK_WIDTH - 3.
        // It has a limit of 1 neighbour chunk, so querying out of the neighbour chunk's bounds will
        // result in out of bounds again, thus, unpredictable results.
        if (coord.y < 0 || coord.y >= CHUNK_HEIGHT) return nullptr;
        if (coord.x < 0) {
            chunkIndex = CHUNK_INDEX_WEST(chunkIndex);
            coord.x = CHUNK_WIDTH + coord.x;
        } else if (coord.x >= CHUNK_WIDTH) {
            chunkIndex = CHUNK_INDEX_EAST(chunkIndex);
            coord.x = coord.x - CHUNK_WIDTH;
        }
        if (coord.z < 0) {
            chunkIndex = CHUNK_INDEX_SOUTH(chunkIndex);
            coord.z = CHUNK_WIDTH + coord.z;
        } else if (coord.z >= CHUNK_WIDTH) {
            chunkIndex = CHUNK_INDEX_NORTH(chunkIndex);
            coord.z = coord.z - CHUNK_WIDTH;
        }
        return &(m_chunks[chunkIndex]->blocks[BLOCK_COORD_TO_INDEX(coord)]);
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

    void VoxelTerrain::GenerateTerrainTexture() {
        std::string texturesPath = "Assets/Textures/Terrain";
        std::vector<std::string> grassTop;
        std::vector<std::string> grassSide;
        std::vector<std::string> grassBottom;
        std::vector<std::string> dirtTop;
        std::vector<std::string> dirtSide;
        std::vector<std::string> dirtBottom;
        std::vector<std::string> allTextures;

        for (const auto & entry : std::filesystem::directory_iterator(texturesPath)) {
            std::string filename = entry.path().string();
            // Grass.
            if (filename.find("grass_top") != std::string::npos) grassTop.push_back(filename);
            else if (filename.find("grass_side") != std::string::npos) grassSide.push_back(filename);
            else if (filename.find("grass_bottom") != std::string::npos) grassBottom.push_back(filename);

            // Dirt.
            else if (filename.find("dirt_top") != std::string::npos) dirtTop.push_back(filename);
            else if (filename.find("dirt_side") != std::string::npos) dirtSide.push_back(filename);
            else if (filename.find("dirt_bottom") != std::string::npos) dirtBottom.push_back(filename);

            allTextures.push_back(filename);
        }
        Core::TextureAtlas* terrainAtlas = Core::LoadTextureAtlas(allTextures);
        Core::ResourceManager::CreateTextureAsset("TerrainTextureAtlas", terrainAtlas->texture);

        BlockTexturesInfo blocksInfos[TOTAL_BLOCKS_IN_GAME];

        // Grass.
        BlockTexturesInfo grassVariations;
        for(auto& texName : grassTop) grassVariations.top_variations.push_back(terrainAtlas->coordinates[texName]);
        for(auto& texName : grassSide) grassVariations.side_variations.push_back(terrainAtlas->coordinates[texName]);
        for(auto& texName : grassBottom) grassVariations.bottom_variations.push_back(terrainAtlas->coordinates[texName]);
        blocksInfos[GRASS_BLOCK_ID] = grassVariations;
        
        // Dirt.
        BlockTexturesInfo dirtVariations;
        for(auto& texName : dirtTop) dirtVariations.top_variations.push_back(terrainAtlas->coordinates[texName]);
        for(auto& texName : dirtSide) dirtVariations.side_variations.push_back(terrainAtlas->coordinates[texName]);
        for(auto& texName : dirtBottom) dirtVariations.bottom_variations.push_back(terrainAtlas->coordinates[texName]);
        blocksInfos[DIRT_BLOCK_ID] = dirtVariations;

        blocksTexturesInfos.assign(blocksInfos, blocksInfos + TOTAL_BLOCKS_IN_GAME);

        delete terrainAtlas;
    }

    void VoxelTerrain::GenerateChunkBlocks(Chunk* chunk, int chunkIndex) {
        glm::vec3 chunkPos = glm::vec3(chunk->id.x, 0, chunk->id.y) * (float)CHUNK_WIDTH;
        glm::vec3 position;
        FOR_BLOCK() {
            position = glm::vec3(x, y, z) + chunkPos;
            int height1 = (glm::sin(position.x * 0.05f) + glm::sin(position.z * 0.072f)) * 5 + 10;
            int height2 = (glm::sin(position.x * 0.0075f) + glm::sin(position.z * 0.0052f)) * 30 + 30;
            if (position.y > height1 + height2) chunk->blocks[index].id = 0;
            else if (position.y == height1 + height2) chunk->blocks[index].id = 1;
            else chunk->blocks[index].id = 2;
        }

        chunk->blocksInitialized = true;
    }

    void VoxelTerrain::GenerateChunkMesh(Chunk* chunk, int chunkIndex) {
        int trisIndex = 0;

        Block* neighBlocks[9];
        srand((unsigned int)(chunk->id.x * chunk->id.y));
        
        FOR_BLOCK() {
            glm::ivec3 position(x, y, z); // For integer usage.
            glm::vec3 pos(x, y, z); // For floats usage.
            if (chunk->blocks[index].id) {
                // EAST
                if ((x != CHUNK_WIDTH - 1 && chunk->blocks[BLOCK_INDEX_EAST(index)].id == 0) ||
                    (x == CHUNK_WIDTH - 1 && m_chunks[CHUNK_INDEX_EAST(chunkIndex)]->blocks[BLOCK_COORD_TO_INDEX(glm::ivec3(0, y, z))].id == 0)) {
                    glm::vec3 faceNormal(1.0, 0.0, 0.0);

                    // Getting texture coordinates.
                    Core::TextureCoordinate texCoords;
                    if (blocksTexturesInfos[chunk->blocks[index].id].side_variations.size() > 0) {
                        int varIdx = rand() % blocksTexturesInfos[chunk->blocks[index].id].side_variations.size();
                        texCoords = blocksTexturesInfos[chunk->blocks[index].id].side_variations[varIdx];
                    } else {
                        int varIdx = rand() % blocksTexturesInfos[chunk->blocks[index].id].top_variations.size();
                        texCoords = blocksTexturesInfos[chunk->blocks[index].id].top_variations[varIdx];
                    }

                    neighBlocks[0] = QueryNeighbourChunkBlockFromLocalCoords(glm::ivec3(x + 1, y + 1, z - 1), chunkIndex);
                    neighBlocks[1] = QueryNeighbourChunkBlockFromLocalCoords(glm::ivec3(x + 1, y + 1, z), chunkIndex);
                    neighBlocks[2] = QueryNeighbourChunkBlockFromLocalCoords(glm::ivec3(x + 1, y + 1, z + 1), chunkIndex);
                    neighBlocks[3] = QueryNeighbourChunkBlockFromLocalCoords(glm::ivec3(x + 1, y, z - 1), chunkIndex);
                    neighBlocks[4] = QueryNeighbourChunkBlockFromLocalCoords(glm::ivec3(x + 1, y, z), chunkIndex);
                    neighBlocks[5] = QueryNeighbourChunkBlockFromLocalCoords(glm::ivec3(x + 1, y, z + 1), chunkIndex);
                    neighBlocks[6] = QueryNeighbourChunkBlockFromLocalCoords(glm::ivec3(x + 1, y - 1, z - 1), chunkIndex);
                    neighBlocks[7] = QueryNeighbourChunkBlockFromLocalCoords(glm::ivec3(x + 1, y - 1, z), chunkIndex);
                    neighBlocks[8] = QueryNeighbourChunkBlockFromLocalCoords(glm::ivec3(x + 1, y - 1, z + 1), chunkIndex);
                    
                    // Bottom Right Vertex.
                    int vertexAOLight = 3;
                    if (neighBlocks[5] && neighBlocks[5]->id != 0 && neighBlocks[7] && neighBlocks[7]->id != 0) vertexAOLight = 0;
                    else vertexAOLight = 3 - ((int)(neighBlocks[5] && neighBlocks[5]->id != 0) +
                        (int)(neighBlocks[7] && neighBlocks[7]->id != 0) +
                        (int)(neighBlocks[8] && neighBlocks[8]->id != 0));
                    m_vertices.push_back(Core::Vertex(pos + glm::vec3(0.5f, -0.5f, 0.5f), faceNormal, texCoords.bottom_right, glm::vec3(vertexAOLight / 3.0f)));
                    
                    // Top Right Vertex.
                    vertexAOLight = 3;
                    if (neighBlocks[5] && neighBlocks[5]->id != 0 && neighBlocks[1] && neighBlocks[1]->id != 0) vertexAOLight = 0;
                    else vertexAOLight = 3 - ((int)(neighBlocks[5] && neighBlocks[5]->id != 0) +
                        (int)(neighBlocks[1] && neighBlocks[1]->id != 0) +
                        (int)(neighBlocks[2] && neighBlocks[2]->id != 0));
                    m_vertices.push_back(Core::Vertex(pos + glm::vec3(0.5f, 0.5f, 0.5f), faceNormal, texCoords.top_right, glm::vec3(vertexAOLight / 3.0f)));

                    // Top Left Vertex.
                    vertexAOLight = 3;
                    if (neighBlocks[1] && neighBlocks[1]->id != 0 && neighBlocks[3] && neighBlocks[3]->id != 0) vertexAOLight = 0;
                    else vertexAOLight = 3 - ((int)(neighBlocks[1] && neighBlocks[1]->id != 0) +
                        (int)(neighBlocks[3] && neighBlocks[3]->id != 0) +
                        (int)(neighBlocks[0] && neighBlocks[0]->id != 0));
                    m_vertices.push_back(Core::Vertex(pos + glm::vec3(0.5f, 0.5f, -0.5f), faceNormal, texCoords.top_left, glm::vec3(vertexAOLight / 3.0f)));

                    // Bottom Left Vertex.
                    vertexAOLight = 3;
                    if (neighBlocks[7] && neighBlocks[7]->id != 0 && neighBlocks[3] && neighBlocks[3]->id != 0) vertexAOLight = 0;
                    else vertexAOLight = 3 - ((int)(neighBlocks[7] && neighBlocks[7]->id != 0) +
                        (int)(neighBlocks[3] && neighBlocks[3]->id != 0) +
                        (int)(neighBlocks[6] && neighBlocks[6]->id != 0));
                    m_vertices.push_back(Core::Vertex(pos + glm::vec3(0.5f, -0.5f, -0.5f), faceNormal, texCoords.bottom_left, glm::vec3(vertexAOLight / 3.0f)));
                    
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
                    (x == 0 && m_chunks[CHUNK_INDEX_WEST(chunkIndex)]->blocks[BLOCK_COORD_TO_INDEX(glm::ivec3(CHUNK_WIDTH - 1, y, z))].id == 0)) {
                    glm::vec3 faceNormal(-1.0, 0.0, 0.0);

                    // Getting texture coordinates.
                    Core::TextureCoordinate texCoords;
                    if (blocksTexturesInfos[chunk->blocks[index].id].side_variations.size() > 0) {
                        int varIdx = rand() % blocksTexturesInfos[chunk->blocks[index].id].side_variations.size();
                        texCoords = blocksTexturesInfos[chunk->blocks[index].id].side_variations[varIdx];
                    } else {
                        int varIdx = rand() % blocksTexturesInfos[chunk->blocks[index].id].top_variations.size();
                        texCoords = blocksTexturesInfos[chunk->blocks[index].id].top_variations[varIdx];
                    }

                    neighBlocks[0] = QueryNeighbourChunkBlockFromLocalCoords(glm::ivec3(x - 1, y + 1, z + 1), chunkIndex);
                    neighBlocks[1] = QueryNeighbourChunkBlockFromLocalCoords(glm::ivec3(x - 1, y + 1, z), chunkIndex);
                    neighBlocks[2] = QueryNeighbourChunkBlockFromLocalCoords(glm::ivec3(x - 1, y + 1, z - 1), chunkIndex);
                    neighBlocks[3] = QueryNeighbourChunkBlockFromLocalCoords(glm::ivec3(x - 1, y, z + 1), chunkIndex);
                    neighBlocks[4] = QueryNeighbourChunkBlockFromLocalCoords(glm::ivec3(x - 1, y, z), chunkIndex);
                    neighBlocks[5] = QueryNeighbourChunkBlockFromLocalCoords(glm::ivec3(x - 1, y, z - 1), chunkIndex);
                    neighBlocks[6] = QueryNeighbourChunkBlockFromLocalCoords(glm::ivec3(x - 1, y - 1, z + 1), chunkIndex);
                    neighBlocks[7] = QueryNeighbourChunkBlockFromLocalCoords(glm::ivec3(x - 1, y - 1, z), chunkIndex);
                    neighBlocks[8] = QueryNeighbourChunkBlockFromLocalCoords(glm::ivec3(x - 1, y - 1, z - 1), chunkIndex);

                    // Bottom Right Vertex.
                    int vertexAOLight = 3;
                    if (neighBlocks[5] && neighBlocks[5]->id != 0 && neighBlocks[7] && neighBlocks[7]->id != 0) vertexAOLight = 0;
                    else vertexAOLight = 3 - ((int)(neighBlocks[5] && neighBlocks[5]->id != 0) +
                        (int)(neighBlocks[7] && neighBlocks[7]->id != 0) +
                        (int)(neighBlocks[8] && neighBlocks[8]->id != 0));
                    m_vertices.push_back(Core::Vertex(pos + glm::vec3(-0.5f, -0.5f, -0.5f), faceNormal, texCoords.bottom_right, glm::vec3(vertexAOLight / 3.0f)));

                    // Top Right Vertex.
                    vertexAOLight = 3;
                    if (neighBlocks[5] && neighBlocks[5]->id != 0 && neighBlocks[1] && neighBlocks[1]->id != 0) vertexAOLight = 0;
                    else vertexAOLight = 3 - ((int)(neighBlocks[5] && neighBlocks[5]->id != 0) +
                        (int)(neighBlocks[1] && neighBlocks[1]->id != 0) +
                        (int)(neighBlocks[2] && neighBlocks[2]->id != 0));
                    m_vertices.push_back(Core::Vertex(pos + glm::vec3(-0.5f, 0.5f, -0.5f), faceNormal, texCoords.top_right, glm::vec3(vertexAOLight / 3.0f)));

                    // Top Left Vertex.
                    vertexAOLight = 3;
                    if (neighBlocks[1] && neighBlocks[1]->id != 0 && neighBlocks[3] && neighBlocks[3]->id != 0) vertexAOLight = 0;
                    else vertexAOLight = 3 - ((int)(neighBlocks[1] && neighBlocks[1]->id != 0) +
                        (int)(neighBlocks[3] && neighBlocks[3]->id != 0) +
                        (int)(neighBlocks[0] && neighBlocks[0]->id != 0));
                    m_vertices.push_back(Core::Vertex(pos + glm::vec3(-0.5f, 0.5f, 0.5f), faceNormal, texCoords.top_left, glm::vec3(vertexAOLight / 3.0f)));

                    // Bottom Left Vertex.
                    vertexAOLight = 3;
                    if (neighBlocks[7] && neighBlocks[7]->id != 0 && neighBlocks[3] && neighBlocks[3]->id != 0) vertexAOLight = 0;
                    else vertexAOLight = 3 - ((int)(neighBlocks[7] && neighBlocks[7]->id != 0) +
                        (int)(neighBlocks[3] && neighBlocks[3]->id != 0) +
                        (int)(neighBlocks[6] && neighBlocks[6]->id != 0));
                    m_vertices.push_back(Core::Vertex(pos + glm::vec3(-0.5f, -0.5f, 0.5f), faceNormal, texCoords.bottom_left, glm::vec3(vertexAOLight / 3.0f)));

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
                    (z == 0 && m_chunks[CHUNK_INDEX_SOUTH(chunkIndex)]->blocks[BLOCK_COORD_TO_INDEX(glm::ivec3(x, y, CHUNK_WIDTH - 1))].id == 0)) {
                    glm::vec3 faceNormal(0.0, 0.0, -1.0);

                    // Getting texture coordinates.
                    Core::TextureCoordinate texCoords;
                    if (blocksTexturesInfos[chunk->blocks[index].id].side_variations.size() > 0) {
                        int varIdx = rand() % blocksTexturesInfos[chunk->blocks[index].id].side_variations.size();
                        texCoords = blocksTexturesInfos[chunk->blocks[index].id].side_variations[varIdx];
                    } else {
                        int varIdx = rand() % blocksTexturesInfos[chunk->blocks[index].id].top_variations.size();
                        texCoords = blocksTexturesInfos[chunk->blocks[index].id].top_variations[varIdx];
                    }

                    neighBlocks[0] = QueryNeighbourChunkBlockFromLocalCoords(glm::ivec3(x - 1, y + 1, z - 1), chunkIndex);
                    neighBlocks[1] = QueryNeighbourChunkBlockFromLocalCoords(glm::ivec3(x, y + 1, z - 1), chunkIndex);
                    neighBlocks[2] = QueryNeighbourChunkBlockFromLocalCoords(glm::ivec3(x + 1, y + 1, z - 1), chunkIndex);
                    neighBlocks[3] = QueryNeighbourChunkBlockFromLocalCoords(glm::ivec3(x - 1, y, z - 1), chunkIndex);
                    neighBlocks[4] = QueryNeighbourChunkBlockFromLocalCoords(glm::ivec3(x, y, z - 1), chunkIndex);
                    neighBlocks[5] = QueryNeighbourChunkBlockFromLocalCoords(glm::ivec3(x + 1, y, z - 1), chunkIndex);
                    neighBlocks[6] = QueryNeighbourChunkBlockFromLocalCoords(glm::ivec3(x - 1, y - 1, z - 1), chunkIndex);
                    neighBlocks[7] = QueryNeighbourChunkBlockFromLocalCoords(glm::ivec3(x, y - 1, z - 1), chunkIndex);
                    neighBlocks[8] = QueryNeighbourChunkBlockFromLocalCoords(glm::ivec3(x + 1, y - 1, z - 1), chunkIndex);

                    // Bottom Right Vertex.
                    int vertexAOLight = 3;
                    if (neighBlocks[5] && neighBlocks[5]->id != 0 && neighBlocks[7] && neighBlocks[7]->id != 0) vertexAOLight = 0;
                    else vertexAOLight = 3 - ((int)(neighBlocks[5] && neighBlocks[5]->id != 0) +
                        (int)(neighBlocks[7] && neighBlocks[7]->id != 0) +
                        (int)(neighBlocks[8] && neighBlocks[8]->id != 0));
                    m_vertices.push_back(Core::Vertex(pos + glm::vec3(0.5f, -0.5f, -0.5f), faceNormal, texCoords.bottom_right, glm::vec3(vertexAOLight / 3.0f)));

                    // Top Right Vertex.
                    vertexAOLight = 3;
                    if (neighBlocks[5] && neighBlocks[5]->id != 0 && neighBlocks[1] && neighBlocks[1]->id != 0) vertexAOLight = 0;
                    else vertexAOLight = 3 - ((int)(neighBlocks[5] && neighBlocks[5]->id != 0) +
                        (int)(neighBlocks[1] && neighBlocks[1]->id != 0) +
                        (int)(neighBlocks[2] && neighBlocks[2]->id != 0));
                    m_vertices.push_back(Core::Vertex(pos + glm::vec3(0.5f, 0.5f, -0.5f), faceNormal, texCoords.top_right, glm::vec3(vertexAOLight / 3.0f)));

                    // Top Left Vertex.
                    vertexAOLight = 3;
                    if (neighBlocks[1] && neighBlocks[1]->id != 0 && neighBlocks[3] && neighBlocks[3]->id != 0) vertexAOLight = 0;
                    else vertexAOLight = 3 - ((int)(neighBlocks[1] && neighBlocks[1]->id != 0) +
                        (int)(neighBlocks[3] && neighBlocks[3]->id != 0) +
                        (int)(neighBlocks[0] && neighBlocks[0]->id != 0));
                    m_vertices.push_back(Core::Vertex(pos + glm::vec3(-0.5f, 0.5f, -0.5f), faceNormal, texCoords.top_left, glm::vec3(vertexAOLight / 3.0f)));

                    // Bottom Left Vertex.
                    vertexAOLight = 3;
                    if (neighBlocks[7] && neighBlocks[7]->id != 0 && neighBlocks[3] && neighBlocks[3]->id != 0) vertexAOLight = 0;
                    else vertexAOLight = 3 - ((int)(neighBlocks[7] && neighBlocks[7]->id != 0) +
                        (int)(neighBlocks[3] && neighBlocks[3]->id != 0) +
                        (int)(neighBlocks[6] && neighBlocks[6]->id != 0));
                    m_vertices.push_back(Core::Vertex(pos + glm::vec3(-0.5f, -0.5f, -0.5f), faceNormal, texCoords.bottom_left, glm::vec3(vertexAOLight / 3.0f)));

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
                    (z == CHUNK_WIDTH - 1 && m_chunks[CHUNK_INDEX_NORTH(chunkIndex)]->blocks[BLOCK_COORD_TO_INDEX(glm::ivec3(x, y, 0))].id == 0)) {
                    glm::vec3 faceNormal(0.0, 0.0, 1.0);

                    // Getting texture coordinates.
                    Core::TextureCoordinate texCoords;
                    if (blocksTexturesInfos[chunk->blocks[index].id].side_variations.size() > 0) {
                        int varIdx = rand() % blocksTexturesInfos[chunk->blocks[index].id].side_variations.size();
                        texCoords = blocksTexturesInfos[chunk->blocks[index].id].side_variations[varIdx];
                    } else {
                        int varIdx = rand() % blocksTexturesInfos[chunk->blocks[index].id].top_variations.size();
                        texCoords = blocksTexturesInfos[chunk->blocks[index].id].top_variations[varIdx];
                    }
                    
                    neighBlocks[0] = QueryNeighbourChunkBlockFromLocalCoords(glm::ivec3(x + 1, y + 1, z + 1), chunkIndex);
                    neighBlocks[1] = QueryNeighbourChunkBlockFromLocalCoords(glm::ivec3(x, y + 1, z + 1), chunkIndex);
                    neighBlocks[2] = QueryNeighbourChunkBlockFromLocalCoords(glm::ivec3(x - 1, y + 1, z + 1), chunkIndex);
                    neighBlocks[3] = QueryNeighbourChunkBlockFromLocalCoords(glm::ivec3(x + 1, y, z + 1), chunkIndex);
                    neighBlocks[4] = QueryNeighbourChunkBlockFromLocalCoords(glm::ivec3(x, y, z + 1), chunkIndex);
                    neighBlocks[5] = QueryNeighbourChunkBlockFromLocalCoords(glm::ivec3(x - 1, y, z + 1), chunkIndex);
                    neighBlocks[6] = QueryNeighbourChunkBlockFromLocalCoords(glm::ivec3(x + 1, y - 1, z + 1), chunkIndex);
                    neighBlocks[7] = QueryNeighbourChunkBlockFromLocalCoords(glm::ivec3(x, y - 1, z + 1), chunkIndex);
                    neighBlocks[8] = QueryNeighbourChunkBlockFromLocalCoords(glm::ivec3(x - 1, y - 1, z + 1), chunkIndex);

                    // Bottom Right Vertex.
                    int vertexAOLight = 3;
                    if (neighBlocks[5] && neighBlocks[5]->id != 0 && neighBlocks[7] && neighBlocks[7]->id != 0) vertexAOLight = 0;
                    else vertexAOLight = 3 - ((int)(neighBlocks[5] && neighBlocks[5]->id != 0) +
                        (int)(neighBlocks[7] && neighBlocks[7]->id != 0) +
                        (int)(neighBlocks[8] && neighBlocks[8]->id != 0));
                    m_vertices.push_back(Core::Vertex(pos + glm::vec3(-0.5f, -0.5f, 0.5f), faceNormal, texCoords.bottom_right, glm::vec3(vertexAOLight / 3.0f)));

                    // Top Right Vertex.
                    vertexAOLight = 3;
                    if (neighBlocks[5] && neighBlocks[5]->id != 0 && neighBlocks[1] && neighBlocks[1]->id != 0) vertexAOLight = 0;
                    else vertexAOLight = 3 - ((int)(neighBlocks[5] && neighBlocks[5]->id != 0) +
                        (int)(neighBlocks[1] && neighBlocks[1]->id != 0) +
                        (int)(neighBlocks[2] && neighBlocks[2]->id != 0));
                    m_vertices.push_back(Core::Vertex(pos + glm::vec3(-0.5f, 0.5f, 0.5f), faceNormal, texCoords.top_right, glm::vec3(vertexAOLight / 3.0f)));

                    // Top Left Vertex.
                    vertexAOLight = 3;
                    if (neighBlocks[1] && neighBlocks[1]->id != 0 && neighBlocks[3] && neighBlocks[3]->id != 0) vertexAOLight = 0;
                    else vertexAOLight = 3 - ((int)(neighBlocks[1] && neighBlocks[1]->id != 0) +
                        (int)(neighBlocks[3] && neighBlocks[3]->id != 0) +
                        (int)(neighBlocks[0] && neighBlocks[0]->id != 0));
                    m_vertices.push_back(Core::Vertex(pos + glm::vec3(0.5f, 0.5f, 0.5f), faceNormal, texCoords.top_left, glm::vec3(vertexAOLight / 3.0f)));

                    // Bottom Left Vertex.
                    vertexAOLight = 3;
                    if (neighBlocks[7] && neighBlocks[7]->id != 0 && neighBlocks[3] && neighBlocks[3]->id != 0) vertexAOLight = 0;
                    else vertexAOLight = 3 - ((int)(neighBlocks[7] && neighBlocks[7]->id != 0) +
                        (int)(neighBlocks[3] && neighBlocks[3]->id != 0) +
                        (int)(neighBlocks[6] && neighBlocks[6]->id != 0));
                    m_vertices.push_back(Core::Vertex(pos + glm::vec3(0.5f, -0.5f, 0.5f), faceNormal, texCoords.bottom_left, glm::vec3(vertexAOLight / 3.0f)));

                    m_triangles.push_back(trisIndex);
                    m_triangles.push_back(trisIndex + 1);
                    m_triangles.push_back(trisIndex + 2);
                    m_triangles.push_back(trisIndex);
                    m_triangles.push_back(trisIndex + 2);
                    m_triangles.push_back(trisIndex + 3);
                    trisIndex += 4;
                }
                // DOWN
                if (y > 0 && chunk->blocks[BLOCK_INDEX_DOWN(index)].id == 0) { 
                    glm::vec3 faceNormal(0.0, -1.0, 0.0);

                    // Getting texture coordinates.
                    Core::TextureCoordinate texCoords;
                    if (blocksTexturesInfos[chunk->blocks[index].id].bottom_variations.size() > 0) {
                        int varIdx = rand() % blocksTexturesInfos[chunk->blocks[index].id].bottom_variations.size();
                        texCoords = blocksTexturesInfos[chunk->blocks[index].id].bottom_variations[varIdx];
                    } else {
                        int varIdx = rand() % blocksTexturesInfos[chunk->blocks[index].id].top_variations.size();
                        texCoords = blocksTexturesInfos[chunk->blocks[index].id].top_variations[varIdx];
                    }
                    
                    neighBlocks[0] = QueryNeighbourChunkBlockFromLocalCoords(glm::ivec3(x + 1, y - 1, z + 1), chunkIndex);
                    neighBlocks[1] = QueryNeighbourChunkBlockFromLocalCoords(glm::ivec3(x, y - 1, z + 1), chunkIndex);
                    neighBlocks[2] = QueryNeighbourChunkBlockFromLocalCoords(glm::ivec3(x - 1, y - 1, z + 1), chunkIndex);
                    neighBlocks[3] = QueryNeighbourChunkBlockFromLocalCoords(glm::ivec3(x + 1, y - 1, z), chunkIndex);
                    neighBlocks[4] = QueryNeighbourChunkBlockFromLocalCoords(glm::ivec3(x, y - 1, z), chunkIndex);
                    neighBlocks[5] = QueryNeighbourChunkBlockFromLocalCoords(glm::ivec3(x - 1, y - 1, z), chunkIndex);
                    neighBlocks[6] = QueryNeighbourChunkBlockFromLocalCoords(glm::ivec3(x + 1, y - 1, z - 1), chunkIndex);
                    neighBlocks[7] = QueryNeighbourChunkBlockFromLocalCoords(glm::ivec3(x, y - 1, z - 1), chunkIndex);
                    neighBlocks[8] = QueryNeighbourChunkBlockFromLocalCoords(glm::ivec3(x - 1, y - 1, z - 1), chunkIndex);

                    // Bottom Right Vertex.
                    int vertexAOLight = 3;
                    if (neighBlocks[5] && neighBlocks[5]->id != 0 && neighBlocks[7] && neighBlocks[7]->id != 0) vertexAOLight = 0;
                    else vertexAOLight = 3 - ((int)(neighBlocks[5] && neighBlocks[5]->id != 0) +
                        (int)(neighBlocks[7] && neighBlocks[7]->id != 0) +
                        (int)(neighBlocks[8] && neighBlocks[8]->id != 0));
                    m_vertices.push_back(Core::Vertex(pos + glm::vec3(-0.5f, -0.5f, -0.5f), faceNormal, texCoords.bottom_right, glm::vec3(vertexAOLight / 3.0f)));

                    // Top Right Vertex.
                    vertexAOLight = 3;
                    if (neighBlocks[5] && neighBlocks[5]->id != 0 && neighBlocks[1] && neighBlocks[1]->id != 0) vertexAOLight = 0;
                    else vertexAOLight = 3 - ((int)(neighBlocks[5] && neighBlocks[5]->id != 0) +
                        (int)(neighBlocks[1] && neighBlocks[1]->id != 0) +
                        (int)(neighBlocks[2] && neighBlocks[2]->id != 0));
                    m_vertices.push_back(Core::Vertex(pos + glm::vec3(-0.5f, -0.5f, 0.5f), faceNormal, texCoords.top_right, glm::vec3(vertexAOLight / 3.0f)));

                    // Top Left Vertex.
                    vertexAOLight = 3;
                    if (neighBlocks[1] && neighBlocks[1]->id != 0 && neighBlocks[3] && neighBlocks[3]->id != 0) vertexAOLight = 0;
                    else vertexAOLight = 3 - ((int)(neighBlocks[1] && neighBlocks[1]->id != 0) +
                        (int)(neighBlocks[3] && neighBlocks[3]->id != 0) +
                        (int)(neighBlocks[0] && neighBlocks[0]->id != 0));
                    m_vertices.push_back(Core::Vertex(pos + glm::vec3(0.5f, -0.5f, 0.5f), faceNormal, texCoords.top_left, glm::vec3(vertexAOLight / 3.0f)));

                    // Bottom Left Vertex.
                    vertexAOLight = 3;
                    if (neighBlocks[7] && neighBlocks[7]->id != 0 && neighBlocks[3] && neighBlocks[3]->id != 0) vertexAOLight = 0;
                    else vertexAOLight = 3 - ((int)(neighBlocks[7] && neighBlocks[7]->id != 0) +
                        (int)(neighBlocks[3] && neighBlocks[3]->id != 0) +
                        (int)(neighBlocks[6] && neighBlocks[6]->id != 0));
                    m_vertices.push_back(Core::Vertex(pos + glm::vec3(0.5f, -0.5f, -0.5f), faceNormal, texCoords.bottom_left, glm::vec3(vertexAOLight / 3.0f)));

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

                    // Getting texture coordinates.
                    Core::TextureCoordinate texCoords;
                    int varIdx = rand() % blocksTexturesInfos[chunk->blocks[index].id].top_variations.size();
                    texCoords = blocksTexturesInfos[chunk->blocks[index].id].top_variations[varIdx];
                    
                    neighBlocks[0] = QueryNeighbourChunkBlockFromLocalCoords(glm::ivec3(x - 1, y + 1, z + 1), chunkIndex);
                    neighBlocks[1] = QueryNeighbourChunkBlockFromLocalCoords(glm::ivec3(x, y + 1, z + 1), chunkIndex);
                    neighBlocks[2] = QueryNeighbourChunkBlockFromLocalCoords(glm::ivec3(x + 1, y + 1, z + 1), chunkIndex);
                    neighBlocks[3] = QueryNeighbourChunkBlockFromLocalCoords(glm::ivec3(x - 1, y + 1, z), chunkIndex);
                    neighBlocks[4] = QueryNeighbourChunkBlockFromLocalCoords(glm::ivec3(x, y + 1, z), chunkIndex);
                    neighBlocks[5] = QueryNeighbourChunkBlockFromLocalCoords(glm::ivec3(x + 1, y + 1, z), chunkIndex);
                    neighBlocks[6] = QueryNeighbourChunkBlockFromLocalCoords(glm::ivec3(x - 1, y + 1, z - 1), chunkIndex);
                    neighBlocks[7] = QueryNeighbourChunkBlockFromLocalCoords(glm::ivec3(x, y + 1, z - 1), chunkIndex);
                    neighBlocks[8] = QueryNeighbourChunkBlockFromLocalCoords(glm::ivec3(x + 1, y + 1, z - 1), chunkIndex);

                    // Bottom Right Vertex.
                    int vertexAOLight = 3;
                    if (neighBlocks[5] && neighBlocks[5]->id != 0 && neighBlocks[7] && neighBlocks[7]->id != 0) vertexAOLight = 0;
                    else vertexAOLight = 3 - ((int)(neighBlocks[5] && neighBlocks[5]->id != 0) +
                        (int)(neighBlocks[7] && neighBlocks[7]->id != 0) +
                        (int)(neighBlocks[8] && neighBlocks[8]->id != 0));
                    m_vertices.push_back(Core::Vertex(pos + glm::vec3(0.5f, 0.5f, -0.5f), faceNormal, texCoords.bottom_right, glm::vec3(vertexAOLight / 3.0f)));

                    // Top Right Vertex.
                    vertexAOLight = 3;
                    if (neighBlocks[5] && neighBlocks[5]->id != 0 && neighBlocks[1] && neighBlocks[1]->id != 0) vertexAOLight = 0;
                    else vertexAOLight = 3 - ((int)(neighBlocks[5] && neighBlocks[5]->id != 0) +
                        (int)(neighBlocks[1] && neighBlocks[1]->id != 0) +
                        (int)(neighBlocks[2] && neighBlocks[2]->id != 0));
                    m_vertices.push_back(Core::Vertex(pos + glm::vec3(0.5f, 0.5f, 0.5f), faceNormal, texCoords.top_right, glm::vec3(vertexAOLight / 3.0f)));

                    // Top Left Vertex.
                    vertexAOLight = 3;
                    if (neighBlocks[1] && neighBlocks[1]->id != 0 && neighBlocks[3] && neighBlocks[3]->id != 0) vertexAOLight = 0;
                    else vertexAOLight = 3 - ((int)(neighBlocks[1] && neighBlocks[1]->id != 0) +
                        (int)(neighBlocks[3] && neighBlocks[3]->id != 0) +
                        (int)(neighBlocks[0] && neighBlocks[0]->id != 0));
                    m_vertices.push_back(Core::Vertex(pos + glm::vec3(-0.5f, 0.5f, 0.5f), faceNormal, texCoords.top_left, glm::vec3(vertexAOLight / 3.0f)));

                    // Bottom Left Vertex.
                    vertexAOLight = 3;
                    if (neighBlocks[7] && neighBlocks[7]->id != 0 && neighBlocks[3] && neighBlocks[3]->id != 0) vertexAOLight = 0;
                    else vertexAOLight = 3 - ((int)(neighBlocks[7] && neighBlocks[7]->id != 0) +
                        (int)(neighBlocks[3] && neighBlocks[3]->id != 0) +
                        (int)(neighBlocks[6] && neighBlocks[6]->id != 0));
                    m_vertices.push_back(Core::Vertex(pos + glm::vec3(-0.5f, 0.5f, -0.5f), faceNormal, texCoords.bottom_left, glm::vec3(vertexAOLight / 3.0f)));

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
            Core::Mesh* mesh = new Core::Mesh();
            mesh->Create((GLfloat*)&m_vertices[0], m_vertices.size(), (GLuint*)&m_triangles[0], m_triangles.size());
            chunk->SetMesh(mesh);
            m_vertices.clear();
            m_triangles.clear();
        } else {
            chunk->SetMesh(nullptr);
        }
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
        if (m_initialized && newPivot != m_pivotChunk) {
            glm::ivec2 delta = newPivot - m_pivotChunk;
            m_pivotChunk = newPivot;
            TriggerChunkMatrixTranslation(delta);
        }
    }

    void VoxelTerrain::Init() {
        m_vertices.reserve(3000);
        m_triangles.reserve(4500);
        GenerateRenderingSpiral();
        m_chunks = new Chunk * [m_renderDistance * m_renderDistance * 4];

        int chunkIndex = 0;
        for (int chunkX = -m_renderDistance; chunkX < m_renderDistance; chunkX++) {
            for (int chunkZ = -m_renderDistance; chunkZ < m_renderDistance; chunkZ++) {
                m_chunks[chunkIndex] = new Chunk(glm::ivec2(chunkX, chunkZ) + m_pivotChunk);
                chunkIndex++;
            }
        }

        GenerateTerrainTexture();
        m_material = Core::ResourceManager::GetOrLoadMaterialAsset("Assets/Materials/TerrainMaterial.mat");
    }

    void VoxelTerrain::Update() {
        double elapsedTime = 0.0;
        for (auto it = m_renderingSpiral.begin(); it != m_renderingSpiral.end() && elapsedTime < 0.016; ++it) {
            auto startTime = std::chrono::high_resolution_clock::now();
            if (!m_chunks[it->z]->blocksInitialized) {
                GenerateChunkBlocks(m_chunks[it->z], it->z);
            } 
            if (!m_chunks[it->z]->meshInitialized) {
                // Cheking if all chunks in a neighbourhood of 3x3 have their blocks already initialized,
                // so that the mesh generation algorithm don't have to worry about that for each block.
                if (it->x != -m_renderDistance && m_chunks[CHUNK_INDEX_WEST(it->z)]->blocksInitialized &&
                    it->x != m_renderDistance - 1 && m_chunks[CHUNK_INDEX_EAST(it->z)]->blocksInitialized &&
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
            elapsedTime += diff.count();
        }
    }

    void VoxelTerrain::Render() {
        m_material->Use();
        glm::mat4 model = ((Core::GameObject*)gameObject)->transform.GetTRSMatrix();
        

        int chunkCount = m_renderDistance * m_renderDistance * 4;
        for (int chunkIndex = 0; chunkIndex < chunkCount; chunkIndex++) {
            if (m_chunks[chunkIndex]->GetMesh() != nullptr) {
                Core::Graphics::SetMesh(m_chunks[chunkIndex]->GetMesh());
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
