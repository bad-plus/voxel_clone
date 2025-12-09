#pragma once
#include "biome.hpp"
#include "perlin_2d.h"

#include <memory>

struct Chunk;

class WorldGenerator {
public:
    WorldGenerator(int seed);
    ~WorldGenerator();

    Chunk* generateChunk(Chunk* chunk, int x, int z);
private:
    void generateTerrain(Chunk* chunk, int x, int z);
    void generateRivers(Chunk* chunk, int x, int z);
    void generateCanyon(Chunk* chunk, int x, int z);
    void generateBedrock(Chunk* chunk, int x, int z);

    int m_seed;
    std::unique_ptr<Perlin2D> m_perlin;

    std::map<Biome::BiomeID, std::unique_ptr<Perlin2D>> m_biome_perlins;

    std::unique_ptr<Perlin2D> m_perlin_river;
    std::unique_ptr<Perlin2D> m_perlin_canyon;
};