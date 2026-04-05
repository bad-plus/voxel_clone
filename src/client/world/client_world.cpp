#include "client_world.h"
#include <core/world/chunk/chunk.h>
#include <core/world/block/block.h>
#include <core/logger.hpp>
#include <GLFW/glfw3.h>
#include <thread>
#include <algorithm>
#include <vector>
#include <memory>
#include <core/ecs/core/ecs.h>
#include <core/constants.h>

#include <core/ecs/systems/player_camera_system.h>
#include <core/ecs/systems/world_collision_system.h>
#include <core/ecs/systems/player_systems_manager.h>
#include <core/ecs/systems/gravity_system.h>
#include <core/ecs/systems/camera_update_system.h>
#include <core/world/events/world_event.h>
#include <core/world/events/world_event_manager.h>
#include "events\client_events.h"

ClientWorld::ClientWorld() {
    m_chunk_creation_time = 0.0;
    last_tick_time = 0;

    m_ecs.ecs = std::make_unique<ECS>();

    m_ecs.player_camera_system = std::make_unique<PlayerCameraSystem>();
    m_ecs.camera_update_system = std::make_unique<CameraUpdateSystem>();
    m_ecs.world_collision_system = std::make_unique<WorldCollisionSystem>();
    m_ecs.gravity_system = std::make_unique<GravitySystem>();
    m_ecs.player_movement_systems = std::make_unique<PlayerSystemsManager>();

    m_event_manager = std::make_unique<WorldEventManager<ClientWorld>>();
}

ClientWorld::~ClientWorld() {
    for (auto& [key, chunk] : m_chunks) {
        if (chunk != nullptr) {
            delete chunk;
        }
    }
}

ClientChunk* ClientWorld::getChunk(int x, int z)
{
    return dynamic_cast<ClientChunk*>(World::getChunk(x, z));
}

Chunk* ClientWorld::createChunk(int x, int z)
{
    const long long chunk_index = ChunkCoord(x, z).getIndex();

    std::lock_guard<std::mutex> lock(m_chunks_mutex);

    auto it = m_chunks.find(chunk_index);
    if (it != m_chunks.end()) {
        return dynamic_cast<Chunk*>(it->second);
    }

    Chunk* new_chunk = new ClientChunk();
    m_chunks[chunk_index] = new_chunk;
    return new_chunk;
}

//ClientChunk* ClientWorld::getChunk(int x, int z, bool create) {
//    ClientChunk* chunk = getChunkProtected(x, z);
//
//    if (chunk != nullptr) return chunk;
//
//    if (!create) {
//        return nullptr;
//    }
//
//    chunk = createChunk(x, z);
//    ChunkCoord coord = { x, z };
//    addEvent(
//        std::make_unique<GenerateChunkEvent>(coord)
//	);
//
//    return chunk;
//}

void ClientWorld::setBlock(int world_x, int world_y, int world_z, BlockID block_id) {
	int chunk_x = floorDiv(world_x, Constants::CHUNK_SIZE_X);
	int chunk_z = floorDiv(world_z, Constants::CHUNK_SIZE_Z);

	int in_chunk_x = world_x % Constants::CHUNK_SIZE_X;
	if (in_chunk_x < 0) in_chunk_x += Constants::CHUNK_SIZE_X;

	int in_chunk_y = world_y;

	int in_chunk_z = world_z % Constants::CHUNK_SIZE_Z;
	if (in_chunk_z < 0) in_chunk_z += Constants::CHUNK_SIZE_Z;

	ClientChunk* chunk = getChunk(chunk_x, chunk_z);
	if (chunk == nullptr) return;

	Block* block = chunk->getBlock({ in_chunk_x, in_chunk_y, in_chunk_z });
	if (block == nullptr) return;

	block->setBlockID(block_id);

	chunk->markDirty();

	addEvent(
		std::make_unique<UpdateMeshEvent>(ChunkCoord(chunk_x, chunk_z))
		, true);

	if (in_chunk_x == 0) {
		auto chunk = getChunk(chunk_x - 1, chunk_z);
		if (chunk != nullptr) {
			chunk->markDirty();
			addEvent(
				std::make_unique<UpdateMeshEvent>(ChunkCoord(chunk_x - 1, chunk_z))
				, true);
		}
	}

	if (in_chunk_x == Constants::CHUNK_SIZE_X - 1) {
		auto chunk = getChunk(chunk_x + 1, chunk_z);
		if (chunk != nullptr) {
			chunk->markDirty();
			addEvent(
				std::make_unique<UpdateMeshEvent>(ChunkCoord(chunk_x + 1, chunk_z))
				, true);
		}
	}

	if (in_chunk_z == 0) {
		auto chunk = getChunk(chunk_x, chunk_z - 1);
		if (chunk != nullptr) {
			chunk->markDirty();
			addEvent(
				std::make_unique<UpdateMeshEvent>(ChunkCoord(chunk_x, chunk_z - 1))
				, true);
		}
	}

	if (in_chunk_z == Constants::CHUNK_SIZE_Z - 1) {
		auto chunk = getChunk(chunk_x, chunk_z + 1);
		if (chunk != nullptr) {
			chunk->markDirty();
			addEvent(
				std::make_unique<UpdateMeshEvent>(ChunkCoord(chunk_x, chunk_z + 1))
				, true);
		}
	}
}

Entity ClientWorld::CreatePlayer() {
    ECS* ecs = getECS();
    Entity entity = ecs->create();

    glm::ivec3 spawn_position = { 0, 0, 0 };
    ChunkCoord chunk_coord = ChunkCoord::fromWorldPosition(spawn_position.x, spawn_position.z);

    ecs->storage<Transform>().add(entity, { spawn_position, {0.0f, 0.0f, 0.0f} });
    ecs->storage<Velocity>().add(entity, { 0.0f, 0.0f, 0.0f });
    ecs->storage<Camera>().add(entity, Camera());
	ecs->storage<PlayerControlledCamera>().add(entity, {});
	ecs->storage<ActiveCamera>().add(entity, {});
    ecs->storage<PlayerInput>().add(entity, PlayerInput());
    ecs->storage<PlayerState>().add(entity, PlayerState());
    ecs->storage<PlayerTag>().add(entity, PlayerTag());
    ecs->storage<Collider>().add(entity, {0.4f, 0.9f, 0.4f});
    ecs->storage<Mass>().add(entity, { 1.0f });
    ecs->storage<PhysicsState>().add(entity, PhysicsState());
    ecs->storage<Movement>().add(entity, Movement());

    return entity;
}

ECS* ClientWorld::getECS() {
    return m_ecs.ecs.get();
}

void ClientWorld::tick() {
    if (last_tick_time == Time{}) {
        last_tick_time = Time::now();
        return;
    }

    Time current_time = Time::now();
    Time delta_time = current_time - last_tick_time;
    last_tick_time = current_time;

    ECS* ecs = m_ecs.ecs.get();

    m_event_manager->process(*this);
}

void ClientWorld::tick_movement() {
    if (last_tick_time == Time{}) {
        last_tick_time = Time::now();
        return;
    }

    Time current_time = Time::now();
    Time delta_time = current_time - last_tick_time;
    last_tick_time = current_time;

    ECS* ecs = m_ecs.ecs.get();

    double dt = delta_time.getS<double>();

    m_ecs.player_camera_system->update(*ecs);

    m_ecs.player_movement_systems->update(*ecs, dt);

    m_ecs.gravity_system->update(*ecs, dt);

    m_ecs.world_collision_system->update(*ecs, dt, this);

    m_ecs.camera_update_system->update(*ecs);
}


Time ClientWorld::getMeshGenerationTime() const {
    auto all_time = Time();
    int all_count = 0;


    for (const auto& [key, value] : m_chunks) {
        if (value == nullptr) continue;

        auto time = static_cast<ClientChunk*>(value)->getChunkBuildTime();

        if (time != 0.0f) {
            all_time += time;
            all_count++;
        }
    }

    if (all_count == 0) return Time(0);
    return (all_time / all_count);
}
std::vector<std::pair<int, int>> genCircleReadyA(int cx, int cz, int radius) {
	std::vector<std::pair<int, int>> out;

	for (int x = -radius; x <= radius; x++) {
		for (int z = -radius; z <= radius; z++) {
			if (x * x + z * z <= radius * radius) {
				out.emplace_back(cx + x, cz + z);
			}
		}
	}

	std::sort(out.begin(), out.end(), [cx, cz](const auto& a, const auto& b) {
		int dist_a = (a.first - cx) * (a.first - cx) + (a.second - cz) * (a.second - cz);
		int dist_b = (b.first - cx) * (b.first - cx) + (b.second - cz) * (b.second - cz);
		return dist_a < dist_b;
		});

	return out;
}


void ClientWorld::addEvent(std::unique_ptr<WorldEvent<ClientWorld>> event, bool priority) {
    m_event_manager->push(std::move(event), priority);
}

void ClientWorld::shutdown()
{
    m_event_manager->shutdown();
    World::shutdown();
}
