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

BiomeID WorldGeneator::getBiome(int x, int z) {
    //const float biome_size = 0.0008f;
    const float biome_size = 0.002f; // test
    float value = m_perlin->noise((float)(x * biome_size), (float)(z * biome_size));
    float normalized_value = ((value + 1.0f) / 2.0f);
    
    float biome_value = (1.0f / (float)BiomeID::END);

    BiomeID result = (BiomeID)((int)(normalized_value / biome_value));
    return result;
}

Chunk* WorldGeneator::generateChunk(Chunk* chunk, int x, int z) {
    for(int px = 0; px < CHUNK_SIZE_X; px++) {
        for(int pz = 0; pz < CHUNK_SIZE_Z; pz++) {
            const float world_height = 130;
            int world_pos_x = (x * CHUNK_SIZE_X) + px;
            int world_pos_z = (z * CHUNK_SIZE_Z) + pz;

            BiomeID biome = getBiome(world_pos_x, world_pos_z);

            float height = 0;

            switch(biome) {
                case BiomeID::PLANKS: {
                    const float detail_size = 0.005f;
                    const float detail_amp = 50;
                    float detail = m_perlin->noise((float)(world_pos_x * detail_size), (float)(world_pos_z * detail_size)) * detail_amp;

                    height = detail;
                    break;
                }
                case BiomeID::WINTER: {
                    const float detail_size = 0.005f;
                    const float detail_amp = 10;
                    float detail = m_perlin->noise((float)(world_pos_x * detail_size), (float)(world_pos_z * detail_size)) * detail_amp;

                    height = detail;
                    break;
                }
                case BiomeID::HILLS: {
                    const float detail_size = 0.05f;
                    const float detail_amp = 60;
                    float detail = m_perlin->noise((float)(world_pos_x * detail_size), (float)(world_pos_z * detail_size)) * detail_amp;

                    height = detail;
                    break;
                }
            }

            height += world_height;

            int final_height = (int)height;

            for(int py = 0; py < final_height; py++) {
                BlockID block = BlockID::STONE;

                if(py == final_height - 1) {
                    if(biome == BiomeID::WINTER) block = BlockID::SNOW_GRASS;
                    else block = BlockID::GRASS;
                }
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