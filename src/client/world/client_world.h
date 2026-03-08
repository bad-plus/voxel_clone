#pragma once
#include <unordered_map>
#include <deque>
#include <mutex>
#include <memory>
#include <core/world/block/block.h>
#include <core/ecs/core/entity.h>
#include <core/world/chunk/chunk.h>
#include <core/time.hpp>
#include <core/world/world.h>
#include "chunk/client_chunk.h"
#include <core/world/events/world_event.h>

struct Chunk;
struct WorldGenerator;
struct ECS;
struct PlayerSystemsManager;
struct WorldCollisionSystem;
struct GravitySystem;
struct CameraUpdateSystem;
struct PlayerCameraSystem;

class ClientWorld : public World {
public:
    ClientWorld();
    ~ClientWorld();

    ClientChunk* getChunk(int x, int z);

    Chunk* createChunk(int x, int z) override;

    void setBlock(int world_x, int world_y, int world_z, BlockID block_id) override;

    Entity CreatePlayer();
    ECS* getECS();

    void tick();
    void tick_movement();

    Time getMeshGenerationTime() const;

    void generateChunks(int chunk_x, int chunk_z, int radius = 1);

    void addEvent(std::unique_ptr<WorldEvent<ClientWorld>> event, bool priority = false);

    void shutdown() override;
private:
    double m_chunk_creation_time;

    std::unique_ptr < WorldEventManager < ClientWorld >> m_event_manager;

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