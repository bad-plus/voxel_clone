#pragma once

struct Chunk;

class WorldGeneator {
public:
    WorldGeneator(int seed);
    ~WorldGeneator();

    Chunk* generateChunk(int x, int z);
private:
    int m_seed;
};