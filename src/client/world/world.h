#pragma once
#include <unordered_map>
#include <deque>
#include <mutex>
#include <memory>
#include <core/world/block/block.h>
#include <core/ecs/core/entity.h>
#include <core/world/chunk/chunk.h>
#include "chunk/client_chunk.h"
#include "world/world_event.h"
#include <core/time.hpp>

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
    ClientChunk* chunk;
    int x;
    int z;
};

class World {
public:
    World(WorldGenerator* generator = nullptr);
    ~World();

    ClientChunk* getChunk(int x, int z, bool create = false);
    Block* getBlock(int world_x, int world_y, int world_z);

    void setBlock(int world_x, int world_y, int world_z, BlockID block_id);

    Entity CreatePlayer();
    ECS* getECS();

    void tick();
    void tick_movement();

    const WorldGenerator* getGenerator() const { return m_generator; }

    Time getMeshGenerationTime() const;

    void generateChunks(int chunk_x, int chunk_z, int radius = 1);

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

    Time last_tick_time;
};