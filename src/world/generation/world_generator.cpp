#include "world_generator.h"
#include "../block.h"
#include "../chunk.h"
#include "../../core/logger.h"
#include "biome.hpp"

WorldGenerator::WorldGenerator(int seed) {
    m_seed = seed;
    m_perlin = new Perlin2D(m_seed);
}
WorldGenerator::~WorldGenerator() {
    delete m_perlin;
}

Chunk* WorldGenerator::generateChunk(Chunk* chunk, int x, int z) {
    generateTerrain(chunk, x, z);
    return chunk;
}