#include "world_generator.h"
#include "../block.h"
#include "../chunk.h"
#include "../../core/logger.h"
#include "biome.hpp"

WorldGenerator::WorldGenerator(int seed) {
    m_seed = seed;
    m_perlin = new Perlin2D(m_seed);

    m_perlin_river = new Perlin2D(m_seed + 1);

    m_biome_perlins[Biome::BiomeID::PLANKS] = new Perlin2D(m_seed + 10);
    m_biome_perlins[Biome::BiomeID::HILLS] = new Perlin2D(m_seed + 11);
    m_biome_perlins[Biome::BiomeID::SHARP_PEAKS] = new Perlin2D(m_seed + 12);
    m_biome_perlins[Biome::BiomeID::WINTER] = new Perlin2D(m_seed + 13);
    m_biome_perlins[Biome::BiomeID::DESERT] = new Perlin2D(m_seed + 14);
}
WorldGenerator::~WorldGenerator() {
    delete m_perlin;
    delete m_perlin_river;

    for (auto& perlin : m_biome_perlins) {
        delete perlin.second;
    }
}

Chunk* WorldGenerator::generateChunk(Chunk* chunk, int x, int z) {
    generateTerrain(chunk, x, z);
    generateRivers(chunk, x, z);
    return chunk;
}