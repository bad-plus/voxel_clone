#include "world_generator.h"
#include "../block.h"
#include "../chunk.h"
#include "biome.hpp"
#include "../../core/logger.h"
#include "perlin_2d.h"

constexpr float frequency_canyon = 0.001f;
constexpr float amplitude_canyon = 130.0f;
constexpr float len_canyon = 0.1f;

constexpr float canyon_start_noise_value = 0.85f;

void WorldGenerator::generateCanyon(Chunk* chunk, int x, int z) {
    for (int px = 0; px < CHUNK_SIZE_X; px++) {
        for (int pz = 0; pz < CHUNK_SIZE_Z; pz++) {
            int world_x = x * CHUNK_SIZE_X + px;
            int world_z = z * CHUNK_SIZE_Z + pz;

            float canyon_noise = (m_perlin_canyon->noise(
                (float)world_x * frequency_canyon,
                (float)world_z * frequency_canyon * len_canyon) + 1.0f) * 0.5f;
            
            if (canyon_noise > canyon_start_noise_value) {
                int height = chunk->getTopBlockPosition(px, pz);

                float noise_normalize = (canyon_noise - canyon_start_noise_value) /
                    (1.0f - canyon_start_noise_value);

                int canyon_depth = (int)(noise_normalize * amplitude_canyon);

                for (int y = height; y > height - canyon_depth && y >= 0; y--) {
                    chunk->setBlock({px, y, pz}, BlockID::EMPTY);
                }
            }
        }
    }
}