#include "world_generator.h"
#include "block.h"
#include "chunk.h"
#include "perlin_2d.h"
#include "../core/logger.h"

WorldGeneator::WorldGeneator(int seed) {
    m_seed = seed;
    m_perlin = new Perlin2D(m_seed);
}
WorldGeneator::~WorldGeneator() {
    delete m_perlin;
}

Chunk* WorldGeneator::generateChunk(Chunk* chunk, int x, int z) {
    const int max_terrain_height = 256;
    const float scale = 0.008f;

    for(int px = 0; px < CHUNK_SIZE_X; px++) {
        for(int pz = 0; pz < CHUNK_SIZE_Z; pz++) {
            int world_pos_x = (x * CHUNK_SIZE_X) + px;
            int world_pos_z = (z * CHUNK_SIZE_Z) + pz;

            float continent = m_perlin->noise((float)(world_pos_x * 0.0008f), (float)(world_pos_z * 0.0008f)) * 50.0f;
            float mountain = m_perlin->noise((float)(world_pos_x * 0.01f), (float)(world_pos_z * 0.01f)) * 90.0f;
            float detail = m_perlin->noise((float)(world_pos_x * 0.05f), (float)(world_pos_z * 0.05f)) * 5.0f;

            float height = continent + mountain + detail;

            height += 180.0f;

            int final_height = (int)((height / 270) * max_terrain_height);

            for(int py = 0; py < final_height; py++) {
                BlockID block = BlockID::STONE;

                if(py == final_height - 1) block = BlockID::GRASS;
                if(py == final_height - 2) block = BlockID::DIRT;
                if(py == final_height - 3) block = BlockID::DIRT;
                if(py == final_height - 4) block = BlockID::DIRT;
                if(py == final_height - 5) block = BlockID::DIRT;

                chunk->setBlock({px, py, pz}, block, false);
            }
        }
    }
    return chunk;
}  