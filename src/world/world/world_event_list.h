#include "world_event.h"
#include <glm/glm.hpp>
#include "../../ecs/core/entity.h"
#include "world_event_manager.h"
#include "../chunk/chunk.h"

struct BreakBlockEvent : WorldEvent {
	glm::ivec3 position;
	Entity actor;

	BreakBlockEvent(glm::ivec3 pos, Entity actor_) : position(pos), actor(actor_) {}
	void apply(World& world, WorldEventManager& manager) override;
};

struct SetupBlockEvent : WorldEvent {
	glm::ivec3 position;
	Entity actor;
	BlockID block_id;

	SetupBlockEvent(glm::ivec3 pos, Entity actor_, BlockID block_id_) : position(pos), actor(actor_), block_id(block_id_) {}

	void apply(World& world, WorldEventManager& manager) override;
};

struct UpdateMeshEvent : WorldEvent {
	ChunkCoord position;

	UpdateMeshEvent(ChunkCoord pos) : position(pos) {}
	void apply(World& world, WorldEventManager& manager) override;
};

struct GenerateChunkEvent : WorldEvent {
	ChunkCoord position;

	GenerateChunkEvent(ChunkCoord pos) : position(pos) {}
	void apply(World& world, WorldEventManager& manager) override;
};