#include "world_event_list.h"
#include <core/world/chunk/chunk_storage.h>
#include "../world.h"
#include <core/logger.hpp>

void BreakBlockEvent::apply(World& world, WorldEventManager& manager)
{
	if (position.y < 0 || position.y >= Constants::CHUNK_SIZE_Y) return;

	world.setBlock(position.x, position.y, position.z, BlockID::EMPTY);
}

void SetupBlockEvent::apply(World& world, WorldEventManager& manager)
{
	if (position.y < 0 || position.y >= Constants::CHUNK_SIZE_Y) return;

	world.setBlock(position.x, position.y, position.z, block_id);
}

void UpdateMeshEvent::apply(World& world, WorldEventManager& manager)
{
	ClientChunk* chunk = world.getChunk(position.x, position.z);

	if (chunk == nullptr) return;

	chunk->calculateMesh();
}

void GenerateChunkEvent::apply(World& world, WorldEventManager& manager) 
{
	ClientChunk* chunk = world.getChunk(position.x, position.z);

	if (chunk == nullptr) {
		chunk = world.createChunk(position.x, position.z)->chunk;
	}

	world.generateChunk(position.x, position.z);
	
	manager.push(
		std::make_unique<UpdateMeshEvent>(ChunkCoord(position.x, position.z))
	);

	{
		auto x_m = world.getChunk(position.x - 1, position.z);
		auto z_m = world.getChunk(position.x, position.z - 1);
		auto x_p = world.getChunk(position.x + 1, position.z);
		auto z_p = world.getChunk(position.x, position.z + 1);

		if (x_m != nullptr) {
			x_m->updateNeighbors(chunk, nullptr, nullptr, nullptr);
			chunk->updateNeighbors(nullptr, nullptr, x_m, nullptr);
			manager.push(
				std::make_unique<UpdateMeshEvent>(ChunkCoord(position.x - 1, position.z))
			);
		}

		if (z_m != nullptr) {
			z_m->updateNeighbors(nullptr, chunk, nullptr, nullptr);
			chunk->updateNeighbors(nullptr, nullptr, nullptr, z_m);
			manager.push(
				std::make_unique<UpdateMeshEvent>(ChunkCoord(position.x, position.z - 1))
			);
		}

		if (x_p != nullptr) {
			x_p->updateNeighbors(nullptr, nullptr, chunk, nullptr);
			chunk->updateNeighbors(x_p, nullptr, nullptr, nullptr);
			manager.push(
				std::make_unique<UpdateMeshEvent>(ChunkCoord(position.x + 1, position.z))
			);
		}

		if (z_p != nullptr) {
			z_p->updateNeighbors(nullptr, nullptr, nullptr, chunk);
			chunk->updateNeighbors(nullptr, z_p, nullptr, nullptr);
			manager.push(
				std::make_unique<UpdateMeshEvent>(ChunkCoord(position.x, position.z + 1))
			);
		}
	}
}
