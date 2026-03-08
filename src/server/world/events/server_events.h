#pragma once
#include <core/world/events/world_event.h>
#include <glm/glm.hpp>
#include <core/ecs/core/entity.h>
#include <core/world/chunk/chunk.h>
#include <vector>

class World;
class ServerWorld;

// Server events

struct GenerateChunkEvent : public WorldEvent<ServerWorld> {
    ChunkCoord position;
    GenerateChunkEvent(ChunkCoord pos) : position(pos) {}
    void apply(ServerWorld& world, WorldEventManager<ServerWorld>& manager) override;
};