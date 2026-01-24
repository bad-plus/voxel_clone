#include "world_generator.h"
#include "../block/block.h"
#include "../chunk/chunk.h"
#include "biome.hpp"
#include "../../core/logger.h"

void WorldGenerator::generateBedrock(Chunk* chunk, int x, int z) {
    for (int px = 0; px < CHUNK_SIZE_X; px++) {
        for (int pz = 0; pz < CHUNK_SIZE_Z; pz++) {
            int world_pos_x = (x * CHUNK_SIZE_X) + px;
            int world_pos_z = (z * CHUNK_SIZE_Z) + pz;

            const float base_frequency = 1.1f;
    
            for (int py = 0; py < 5; py++) {
                float noise = (m_perlin->noise(
                    (float)world_pos_x * base_frequency + (py * 1000.f),
                    (float)world_pos_z * base_frequency - (py * 1000.f)) + 1.0f) * 0.5f;

                if (py == 0 || noise > (0.6f)) {
                    chunk->setBlock({ px, py, pz }, BlockID::BEDROCK);
                }
            }
        }
    }
}