#include "world_generator.h"
#include <core/world/block/block.h>
#include <core/world/chunk/chunk.h>
#include "biome.hpp"
#include <core/logger.hpp>
#include "perlin_2d.h"
#include <cmath>
#include <core/constants.h>

const float frenquency_rivers = 0.001f;
const float amplitude_rivers = 15.0f;
const float river_width = 0.02f;

void WorldGenerator::generateRivers(Chunk* chunk, int x, int z) {
    const float river_center = 0.5f;
    const float river_half_width = river_width / 2.0f;
    const float start_river_noise_value = river_center - river_half_width;
    const float end_river_noise_value = river_center + river_half_width;

    for (int px = 0; px < Constants::CHUNK_SIZE_X; px++) {
        for (int pz = 0; pz < Constants::CHUNK_SIZE_Z; pz++) {
            int world_x = x * Constants::CHUNK_SIZE_X + px;
            int world_z = z * Constants::CHUNK_SIZE_Z + pz;

            float river_noise = (m_perlin_river->noise(
                (float)world_x * frenquency_rivers,
                (float)world_z * frenquency_rivers) + 1.0f) * 0.5f;

            if (river_noise > start_river_noise_value && river_noise < end_river_noise_value) {
                int height = chunk->getTopBlockPosition(px, pz);

                float distance_from_center = std::abs(river_noise - river_center) / river_half_width;

                float depth_factor = 1.0f - distance_from_center;

                depth_factor = depth_factor * depth_factor;

                int river_depth = (int)(depth_factor * amplitude_rivers);
                int water_level = height - river_depth;

                for (int py = water_level; py <= height; py++) {
                    chunk->setBlock({ px, py, pz }, BlockID::WATER);
                }

                for (int py = water_level - 1; py >= water_level - 3 && py >= 0; py--) {
                    chunk->setBlock({ px, py, pz }, BlockID::SAND);
                }
            }
        }
    }
}