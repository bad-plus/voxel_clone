#pragma once
#include <unordered_map>
#include <deque>
#include <mutex>
#include <memory>
#include "block/block.h"
#include "../ecs/core/entity.h"
#include "chunk/chunk.h"
#include "world/world_event.h"

struct Chunk;
struct WorldGenerator;
struct ECS;
struct PlayerSystemsManager;
struct WorldCollisionSystem;
struct GravitySystem;
struct CameraUpdateSystem;
struct PlayerCameraSystem;
struct WorldEventManager;

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

    Entity CreatePlayer();
    ECS* getECS();

    void tick();
    void tick_movement();

    const WorldGenerator* getGenerator() const { return m_generator; }

    double getMeshGenerationTime() const;

    void generateChunks(int chunk_x, int chunk_z, int radius = 1);

    static ChunkCoord worldToChunkCoords(int world_x, int world_z);

    ChunkInfo* createChunk(int x, int z);

    void generateChunk(int x, int z);

    void addEvent(std::unique_ptr<WorldEvent> event, bool priority = false);

    void shutdown();
private:
    
    ChunkInfo* getChunkProtected(int x, int z);
    double m_chunk_creation_time;

    std::unordered_map<long long, ChunkInfo*> m_chunks;
    std::mutex m_chunks_mutex;

    WorldGenerator* m_generator;

    std::unique_ptr<WorldEventManager> m_event_manager;

    struct {
        std::unique_ptr<ECS> ecs;
        std::unique_ptr<PlayerCameraSystem> player_camera_system;
        std::unique_ptr<PlayerSystemsManager> player_movement_systems;
        std::unique_ptr<WorldCollisionSystem> world_collision_system;
        std::unique_ptr<GravitySystem> gravity_system;
        std::unique_ptr<CameraUpdateSystem> camera_update_system;
    } m_ecs;

    double last_tick_time;
};