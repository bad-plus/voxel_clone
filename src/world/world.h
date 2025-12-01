#pragma once
#include <unordered_map>

struct Chunk;
struct GameContext;

struct ChunkInfo {
    Chunk* chunk;
};

class World {
public:
    World(GameContext* context);
    ~World();

    Chunk* getChunk(int x, int z, bool create = false);

    void updateMeshChunks();
private:
    ChunkInfo* createChunk();
    double m_chunk_creation_time;
    std::unordered_map<long long, ChunkInfo*> m_chunks;

    GameContext* m_game_context;
};