#pragma once
#include <unordered_map>
#include <deque>
#include <mutex>
#include <memory>
#include "../world/block.h"
#include "../ecs/core/entity.h"

struct Chunk;
struct GameContext;
struct WorldGenerator;
struct ECS;
struct CameraSystem;
struct PlayerMovementSystem;
struct WorldCollisionSystem;
struct GravitySystem;

struct ChunkInfo {
    Chunk* chunk;
    int x;
    int z;
};

class World {
public:
    World(WorldGenerator* generator);
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

    Entity CreatePlayer();
    ECS* getECS();

    void tick();
    void tick_movement();
private:
    ChunkInfo* getChunkProtected(int x, int z);

    ChunkInfo* createChunk(int x, int z);
    double m_chunk_creation_time;

    std::unordered_map<long long, ChunkInfo*> m_chunks;
    std::mutex m_chunks_mutex;

    WorldGenerator* m_generator;

    std::deque<ChunkInfo*> m_generation_queue;
    std::mutex m_generation_queue_mtx;

    std::deque<ChunkInfo*> m_update_mesh_queue;
    std::mutex m_update_mesh_queue_mutex;

    struct {
        std::unique_ptr<ECS> ecs;
        std::unique_ptr<PlayerMovementSystem> player_movement_system;
        std::unique_ptr<WorldCollisionSystem> world_collision_system;
        std::unique_ptr<GravitySystem> gravity_system;
    } m_ecs;

    double last_tick_time;
};