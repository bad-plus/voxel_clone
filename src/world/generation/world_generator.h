#pragma once
#include "biome.hpp"

struct Chunk;
struct Perlin2D;

class WorldGenerator {
public:
    WorldGenerator(int seed);
    ~WorldGenerator();

    Chunk* generateChunk(Chunk* chunk, int x, int z);
private:
    void generateTerrain(Chunk* chunk, int x, int z);
    void generateRivers(Chunk* chunk, int x, int z);
    void generateBedrock(Chunk* chunk, int x, int z);

    int m_seed;
    Perlin2D* m_perlin;

    std::map<Biome::BiomeID, Perlin2D*> m_biome_perlins;

    Perlin2D* m_perlin_river;
};