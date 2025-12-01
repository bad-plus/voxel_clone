#pragma once
#include <unordered_map>

struct Chunk;
struct GameContext;
struct WorldGeneator;

struct ChunkInfo {
    Chunk* chunk;
};

class World {
public:
    World(GameContext* context, WorldGeneator* generator);
    ~World();

    Chunk* getChunk(int x, int z, bool create = false);

    void updateMeshChunks();
private:
    ChunkInfo* createChunk(int x, int z);
    double m_chunk_creation_time;
    std::unordered_map<long long, ChunkInfo*> m_chunks;

    WorldGeneator* m_generator;
    GameContext* m_game_context;
};