#pragma once
#include "biome.hpp"
#include "perlin_2d.h"

#include <memory>
#include <core/time.hpp>

enum class TEMPERATURE_T {
    VERY_COLD = 0,
    COLD,
    NORMAL,
    HOT,
    VERY_HOT,
    END
};

struct Chunk;

class WorldGenerator {
public:
    WorldGenerator(int seed);
    ~WorldGenerator();

    Chunk* generateChunk(Chunk* chunk, int x, int z);

    Time getChunkGenerationTime() const { return m_chunk_generation_time; }
private:
    TEMPERATURE_T getTemperature(int world_block_x, int world_block_z);

    void generateTerrain(Chunk* chunk, int x, int z);
    void generateRivers(Chunk* chunk, int x, int z);
    void generateCanyon(Chunk* chunk, int x, int z);
    void generateBedrock(Chunk* chunk, int x, int z);

    int m_seed;
    std::unique_ptr<Perlin2D> m_perlin;

    std::map<Biome::BiomeID, std::unique_ptr<Perlin2D>> m_biome_perlins;

    std::unique_ptr<Perlin2D> m_perlin_river;
    std::unique_ptr<Perlin2D> m_perlin_canyon;
    std::unique_ptr<Perlin2D> m_perlin_temperature;

    Time m_chunk_generation_time;
};