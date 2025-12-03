#include "world_generator.h"
#include "block.h"
#include "chunk.h"
#include "perlin_2d.h"
#include "../core/logger.h"
#include "biome.hpp"

WorldGeneator::WorldGeneator(int seed) {
    m_seed = seed;
    m_perlin = new Perlin2D(m_seed);
    Biome::autoAssingBiomeRanges();
}
WorldGeneator::~WorldGeneator() {
    delete m_perlin;
}

Chunk* WorldGeneator::generateChunk(Chunk* chunk, int x, int z) {
    for(int px = 0; px < CHUNK_SIZE_X; px++) {
        for(int pz = 0; pz < CHUNK_SIZE_Z; pz++) {
            int world_pos_x = (x * CHUNK_SIZE_X) + px;
            int world_pos_z = (z * CHUNK_SIZE_Z) + pz;

            auto weights = Biome::getBiomeWeight(m_perlin, world_pos_x, world_pos_z);
            Biome::BiomeID biome = Biome::getDominantBiome(weights);
            int height = Biome::getHeight(m_perlin, world_pos_x, world_pos_z, weights);

            for(int py = 0; py < height; py++) {
                BlockID block = BlockID::STONE;

                if(py == height - 1) block = Biome::BiomesInfo[biome].top_block;
                else if (py < height - 1 && py > height - 6) block = Biome::BiomesInfo[biome].under_block;
                else block = BlockID::STONE;

                chunk->setBlock({px, py, pz}, block, false);
            }
        }
    }
    return chunk;
}  