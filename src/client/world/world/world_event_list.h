#pragma once
#include "world_event.h"
#include <glm/glm.hpp>
#include <core/ecs/core/entity.h>
#include <core/world/chunk/chunk.h>
#include <vector>

class World;
class ClientWorld;
class ServerWorld;

// Global events

template<typename W>
struct BreakBlockEvent : public WorldEvent<W> {
    glm::ivec3 position;
    Entity actor;

    BreakBlockEvent(glm::ivec3 pos, Entity actor_) : position(pos), actor(actor_) {}

    void apply(W& world, WorldEventManager<W>& manager) override {
        if (position.y < 0 || position.y >= 256) return;
        world.setBlock(position.x, position.y, position.z, BlockID::EMPTY);
    }
};

template<typename W>
struct SetupBlockEvent : public WorldEvent<W> {
    glm::ivec3 position;
    Entity actor;
    BlockID block_id;

    SetupBlockEvent(glm::ivec3 pos, Entity actor_, BlockID block_id_)
        : position(pos), actor(actor_), block_id(block_id_) {
    }

    void apply(W& world, WorldEventManager<W>& manager) override {
        if (position.y < 0 || position.y >= 256) return;
        world.setBlock(position.x, position.y, position.z, block_id);
    }
};

// Client events

struct UpdateMeshEvent : public WorldEvent<ClientWorld> {
    ChunkCoord position;
    UpdateMeshEvent(ChunkCoord pos) : position(pos) {}
    void apply(ClientWorld& world, WorldEventManager<ClientWorld>& manager) override;
};

struct LoadChunkFromServerEvent : public WorldEvent<ClientWorld> {
    ChunkCoord position;
    std::vector<uint8_t> chunk_bytes;

    LoadChunkFromServerEvent(ChunkCoord pos, const std::vector<uint8_t>& data)
        : position(pos), chunk_bytes(data) {
    }
    void apply(ClientWorld& world, WorldEventManager<ClientWorld>& manager) override;
};

// Server events

struct GenerateChunkEvent : public WorldEvent<ServerWorld> {
    ChunkCoord position;
    GenerateChunkEvent(ChunkCoord pos) : position(pos) {}
    void apply(ServerWorld& world, WorldEventManager<ServerWorld>& manager) override;
};