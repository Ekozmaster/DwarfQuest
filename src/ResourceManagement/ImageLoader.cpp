#include <algorithm>
#include <glm/glm.hpp>
#include <src/ResourceManagement/ImageLoader.h>

namespace DwarfQuest {
    namespace Core {

        typedef struct {
            std::string name;
            int width;
            int height;
            int area;
            int nrChannels;
            unsigned char* data;
        } STBImageTextureData;

        Texture* LoadTexture(const char* path) {
            int width, height, nrChannels;
            stbi_set_flip_vertically_on_load(true);
            unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
            Texture* texture = new Texture();

            texture->Create(data, width, height, nrChannels > 3 ? GL_RGBA : GL_RGB, GL_LINEAR);
            stbi_image_free(data);
            return texture;
        }

        TextureAtlas* LoadTextureAtlas(std::vector<std::string> texturesPaths) {
            std::vector<STBImageTextureData> loadedTexturesData;

            int totalArea = 0;

            stbi_set_flip_vertically_on_load(true);
            for (auto it : texturesPaths) {
                STBImageTextureData textureData;
                textureData.name = it;
                int width, height, nrChannels;
                unsigned char* data = stbi_load(it.c_str(), &textureData.width, &textureData.height, &textureData.nrChannels, 0);
                textureData.data = data;
                textureData.area = textureData.width * textureData.height;
                loadedTexturesData.push_back(textureData);
                totalArea += textureData.area;
            }
            std::sort(loadedTexturesData.begin(), loadedTexturesData.end(), [](const STBImageTextureData& a, const STBImageTextureData& b) { return a.area > b.area; });

            // Texture atlas are, by design, aways in powers of 2. That makes easier to compute their size by simply
            // taking the sqrt of totalArea and then finding the next power of 2.
            unsigned int atlasSize = (unsigned int)glm::sqrt(totalArea);
            atlasSize--;
            atlasSize |= atlasSize >> 1;
            atlasSize |= atlasSize >> 2;
            atlasSize |= atlasSize >> 4;
            atlasSize |= atlasSize >> 8;
            atlasSize |= atlasSize >> 16;
            atlasSize++;

            unsigned char* textureAtlasData = (unsigned char*)malloc(sizeof(unsigned char) * atlasSize * atlasSize * 4);
            for (int i=0; i < atlasSize * atlasSize; i++) textureAtlasData[i] = 0;

            std::map<std::string, TextureCoordinate> coordinates;

            int slotsSize = loadedTexturesData[0].width;
            int columns = (atlasSize / slotsSize);
            std::vector<int> availableSlots(columns * columns);
            for (int i=0; i<availableSlots.size(); i++) availableSlots[i] = i;
            

            for (auto& textureData : loadedTexturesData) {
                // Scalling down nodes in the binary tree of slots within the texture atlas.
                if (textureData.width != slotsSize) {
                    int subdivisions = slotsSize / textureData.width;
                    std::vector<int> newSlots;
                    int newColumns = atlasSize / textureData.width;
                    for (auto slot : availableSlots) {
                        int xPosI = (slot % columns) * subdivisions;
                        int xPosF = xPosI + subdivisions;
                        int yPosI = (slot / columns) * subdivisions;
                        int yPosF = yPosI + subdivisions;

                        for (int y=yPosI; y < yPosF; y++) for (int x=xPosI; x < xPosF; x++) newSlots.push_back(y * newColumns + x);
                    }

                    availableSlots = newSlots;
                    columns = newColumns;
                    slotsSize = textureData.width;
                }

                // Copying data from the texture into the atlas.
                int slot = availableSlots[0];
                int xPosI = (slot % columns) * slotsSize;
                int yPosI = (slot / columns) * slotsSize;
                int texPos = 0;
                for (int y=0; y < slotsSize; y++) {
                    int atlasPos = ((yPosI + y) * atlasSize + xPosI) * 4;
                    for (int x=0; x < slotsSize; x++) {
                        if (textureData.nrChannels == 4) {
                            textureAtlasData[atlasPos] = textureData.data[texPos];
                            textureAtlasData[atlasPos + 1] = textureData.data[texPos + 1];
                            textureAtlasData[atlasPos + 2] = textureData.data[texPos + 2];
                            textureAtlasData[atlasPos + 3] = textureData.data[texPos + 3];
                            texPos += 4;
                        } else {
                            textureAtlasData[atlasPos] = textureData.data[texPos];
                            textureAtlasData[atlasPos + 1] = textureData.data[texPos + 1];
                            textureAtlasData[atlasPos + 2] = textureData.data[texPos + 2];
                            textureAtlasData[atlasPos + 3] = 255;
                            texPos += 3;
                        }
                        atlasPos += 4;
                    }
                }

                // Storing the coordinates.
                TextureCoordinate texCoords;
                texCoords.top_left = glm::vec2(xPosI, yPosI + slotsSize) / (float)atlasSize;
                texCoords.top_right = glm::vec2(xPosI + slotsSize, yPosI + slotsSize) / (float)atlasSize;
                texCoords.bottom_left = glm::vec2(xPosI, yPosI) / (float)atlasSize;
                texCoords.bottom_right = glm::vec2(xPosI + slotsSize, yPosI) / (float)atlasSize;
                coordinates[textureData.name] = texCoords;
                
                // Removing current slot as it has just been occupied.
                availableSlots.erase(availableSlots.begin());
            }
            
            for(auto& texture : loadedTexturesData) stbi_image_free(texture.data);
            TextureAtlas* textureAtlas = new TextureAtlas();
            textureAtlas->texture = new Texture();
            textureAtlas->texture->Create(textureAtlasData, atlasSize, atlasSize, GL_RGBA, GL_NEAREST);
            textureAtlas->coordinates = coordinates;

            free(textureAtlasData);

            return textureAtlas;
        }

    }
}