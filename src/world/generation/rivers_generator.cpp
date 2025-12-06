#include "world_generator.h"
#include "../block.h"
#include "../chunk.h"
#include "biome.hpp"
#include "../../core/logger.h"
#include "perlin_2d.h"

const float frenquency_rivers = 0.001f;
const float amplitude_rivers = 30.0f;

const float start_river_noise_value = 0.49f;
const float end_river_noise_value = 0.5f;

void WorldGenerator::generateRivers(Chunk* chunk, int x, int z) {
    for (int px = 0; px < CHUNK_SIZE_X; px++) {
        for (int pz = 0; pz < CHUNK_SIZE_Z; pz++) {
            int world_x = x * CHUNK_SIZE_X + px;
            int world_z = z * CHUNK_SIZE_Z + pz;

            float river_noise = (m_perlin_river->noise(
                (float)world_x * frenquency_rivers,
                (float)world_z * frenquency_rivers) + 1.0f) * 0.5f;

            if (river_noise > start_river_noise_value && river_noise < end_river_noise_value) {
                int height = chunk->getTopBlockPosition(px, pz);

                float noise_normalize = (river_noise - start_river_noise_value) /
                    (end_river_noise_value - start_river_noise_value);

                int river_depth = (int)(noise_normalize * amplitude_rivers);
                int water_level = height - river_depth;

                for (int py = water_level; py <= height; py++) {
                    chunk->setBlock({ px, py, pz }, BlockID::WATER);
                }

                for (int py = water_level; py < water_level - 3; py++) {
                    chunk->setBlock({ px, py, pz }, BlockID::SAND);
                }
            }

        }
    }
}