#pragma once
#include <core/world/events/world_event.h>
#include <glm/glm.hpp>
#include <core/ecs/core/entity.h>
#include <core/world/chunk/chunk.h>
#include <vector>

class World;
class ClientWorld;

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
