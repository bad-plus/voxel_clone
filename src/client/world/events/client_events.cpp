#include <core/world/events/world_event.h>
#include "client_events.h"
#include "../client_world.h"

void UpdateMeshEvent::apply(ClientWorld& world, WorldEventManager<ClientWorld>& manager) {
	auto* chunk = world.getChunk(position.x, position.z);
	if (chunk) chunk->calculateMesh();
}

void LoadChunkFromServerEvent::apply(ClientWorld& world, WorldEventManager<ClientWorld>& manager) {
	auto* chunk = world.getChunk(position.x, position.z);
	if (chunk == nullptr) {
		world.createChunk(position.x, position.z);
		chunk = world.getChunk(position.x, position.z);
	}

	world.loadFromBytes(position, chunk_bytes);
	world.addEvent(std::make_unique<UpdateMeshEvent>(position));
	{
		auto x_m = world.getChunk(position.x - 1, position.z);
		auto z_m = world.getChunk(position.x, position.z - 1);
		auto x_p = world.getChunk(position.x + 1, position.z);
		auto z_p = world.getChunk(position.x, position.z + 1);

		if (x_m != nullptr) {
			x_m->updateNeighbors(chunk, nullptr, nullptr, nullptr);
			chunk->updateNeighbors(nullptr, nullptr, x_m, nullptr);
			world.addEvent(std::make_unique<UpdateMeshEvent>(ChunkCoord(position.x - 1, position.z)));
		}

		if (z_m != nullptr) {
			z_m->updateNeighbors(nullptr, chunk, nullptr, nullptr);
			chunk->updateNeighbors(nullptr, nullptr, nullptr, z_m);
			world.addEvent(std::make_unique<UpdateMeshEvent>(ChunkCoord(position.x, position.z - 1)));
		}

		if (x_p != nullptr) {
			x_p->updateNeighbors(nullptr, nullptr, chunk, nullptr);
			chunk->updateNeighbors(x_p, nullptr, nullptr, nullptr);
			world.addEvent(std::make_unique<UpdateMeshEvent>(ChunkCoord(position.x + 1, position.z)));
		}

		if (z_p != nullptr) {
			z_p->updateNeighbors(nullptr, nullptr, nullptr, chunk);
			chunk->updateNeighbors(nullptr, z_p, nullptr, nullptr);
			world.addEvent(std::make_unique<UpdateMeshEvent>(ChunkCoord(position.x, position.z + 1)));
		}
	}
}
