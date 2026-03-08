#pragma once
#include <core/world/events/world_event.h>
#include <glm/glm.hpp>
#include <core/ecs/core/entity.h>
#include <core/world/chunk/chunk.h>
#include <vector>

class World;

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