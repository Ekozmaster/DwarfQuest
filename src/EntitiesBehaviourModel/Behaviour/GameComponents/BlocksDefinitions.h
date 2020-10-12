#pragma once

#include <vector>
#include <src/ResourceManagement/ImageLoader.h>

#define TOTAL_BLOCKS_IN_GAME 3  // Including air.
#define AIR_BLOCK_ID 0
#define GRASS_BLOCK_ID 1
#define DIRT_BLOCK_ID 2

namespace DwarfQuest {
    namespace GameComponents {
        typedef struct {
            // Top variations will be used instead when there is no textures in bottom or side variations.
            std::vector<Core::TextureCoordinate> top_variations;
            std::vector<Core::TextureCoordinate> side_variations;
            std::vector<Core::TextureCoordinate> bottom_variations;
        } BlockTexturesInfo;

    }
}