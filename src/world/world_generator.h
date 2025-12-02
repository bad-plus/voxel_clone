#pragma once

struct Chunk;
struct Perlin2D;

enum class BiomeID {
    PLANKS = 0,
    WINTER,
    HILLS,
    //OCEAN,
    END
};

class WorldGeneator {
public:
    WorldGeneator(int seed);
    ~WorldGeneator();

    Chunk* generateChunk(Chunk* chunk, int x, int z);
private:
    BiomeID getBiome(int x, int z);

    int m_seed;
    Perlin2D* m_perlin;
};