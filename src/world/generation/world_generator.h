#pragma once

struct Chunk;
struct Perlin2D;

class WorldGenerator {
public:
    WorldGenerator(int seed);
    ~WorldGenerator();

    Chunk* generateChunk(Chunk* chunk, int x, int z);
private:
    void generateTerrain(Chunk* chunk, int x, int z);

    int m_seed;
    Perlin2D* m_perlin;
};