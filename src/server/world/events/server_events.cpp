#include <core/world/events/world_event.h>
#include "server_events.h"
#include "../server_world.h"

void GenerateChunkEvent::apply(ServerWorld& world, WorldEventManager<ServerWorld>& manager) {
	if (world.getChunk(position.x, position.z) == nullptr) {
		world.createChunk(position.x, position.z);
	}
	world.generateChunk(position.x, position.z);
}
