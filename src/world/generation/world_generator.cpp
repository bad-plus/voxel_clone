#include "world_generator.h"
#include "../block.h"
#include "../chunk.h"
#include "../../core/logger.h"
#include "biome.hpp"

WorldGenerator::WorldGenerator(int seed) {
    m_seed = seed;
    m_perlin = std::make_unique<Perlin2D>(m_seed);

    m_perlin_river = std::make_unique<Perlin2D>(m_seed + 1);
    m_perlin_canyon = std::make_unique<Perlin2D>(m_seed + 2);

    m_biome_perlins[Biome::BiomeID::PLANKS] = std::make_unique<Perlin2D>(m_seed + 10);
    m_biome_perlins[Biome::BiomeID::HILLS] = std::make_unique<Perlin2D>(m_seed + 11);
    m_biome_perlins[Biome::BiomeID::SHARP_PEAKS] = std::make_unique<Perlin2D>(m_seed + 12);
    m_biome_perlins[Biome::BiomeID::WINTER] = std::make_unique<Perlin2D>(m_seed + 13);
    m_biome_perlins[Biome::BiomeID::DESERT] = std::make_unique<Perlin2D>(m_seed + 14);
}
WorldGenerator::~WorldGenerator() = default;

Chunk* WorldGenerator::generateChunk(Chunk* chunk, int x, int z) {
    generateTerrain(chunk, x, z);
    generateTerrain(chunk, x, z);
    generateRivers(chunk, x, z);
    generateCanyon(chunk, x, z);
    generateBedrock(chunk, x, z);
    return chunk;
}