#pragma once
#include <unordered_map>
#include <deque>
#include <mutex>
#include "../world/block.h"

struct Chunk;
struct GameContext;
struct WorldGenerator;

struct ChunkInfo {
    Chunk* chunk;
    int x;
    int z;
};

class World {
public:
    World(GameContext* context, WorldGenerator* generator);
    ~World();

    Chunk* getChunk(int x, int z, bool create = false);
    Block* getBlock(int world_x, int world_y, int world_z);
    void setBlock(int world_x, int world_y, int world_z, BlockID block_id);

    void pushGenerationQueue(ChunkInfo* chunk);
    ChunkInfo* pullGenerationQueue();

    void pushToChunks(ChunkInfo* chunk_info);

    void processGenerationQueue();

    void pushUpdateMeshQueue(ChunkInfo* chunk_info, bool priority = false);
    ChunkInfo* pullUpdateMeshQueue();
    void processUpdateMeshQueue();
private:
    ChunkInfo* getChunkProtected(int x, int z);

    ChunkInfo* createChunk(int x, int z);
    double m_chunk_creation_time;

    std::unordered_map<long long, ChunkInfo*> m_chunks;
    std::mutex m_chunks_mutex;

    WorldGenerator* m_generator;
    GameContext* m_game_context;

    std::deque<ChunkInfo*> m_generation_queue;
    std::mutex m_generation_queue_mtx;

    std::deque<ChunkInfo*> m_update_mesh_queue;
    std::mutex m_update_mesh_queue_mutex;
};