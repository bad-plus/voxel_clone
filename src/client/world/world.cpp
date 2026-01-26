#include "world.h"
#include "chunk/chunk.h"
#include <core/world/block/block.h>
#include <core/logger.hpp>
#include <GLFW/glfw3.h>
#include "generation/world_generator.h"
#include <thread>
#include <algorithm>
#include <vector>
#include "../ecs/core/ecs.h"
#include <core/constants.h>

#include "../ecs/systems/player_camera_system.h"
#include "../ecs/systems/world_collision_system.h"
#include "../ecs/systems/player_systems_manager.h"
#include "../ecs/systems/gravity_system.h"
#include "../ecs/systems/camera_update_system.h"
#include "world/world_event_manager.h"
#include "world/world_event_list.h"

static constexpr long long pack_chunk_coords(int x, int z) {
    return ((long long)(x & 0xFFFFFFFF)) | ((long long)(z & 0xFFFFFFFF) << 32);
}

static constexpr int unpack_chunk_x(long long key) {
    return (int)(key & 0xFFFFFFFF);
}

static constexpr int unpack_chunk_z(long long key) {
    return (int)((key >> 32) & 0xFFFFFFFF);
}

World::World(WorldGenerator* generator) {
    m_generator = generator;
    m_chunk_creation_time = 0.0;
    last_tick_time = 0;

    m_ecs.ecs = std::make_unique<ECS>();

    m_ecs.player_camera_system = std::make_unique<PlayerCameraSystem>();
    m_ecs.camera_update_system = std::make_unique<CameraUpdateSystem>();
    m_ecs.world_collision_system = std::make_unique<WorldCollisionSystem>();
    m_ecs.gravity_system = std::make_unique<GravitySystem>();
    m_ecs.player_movement_systems = std::make_unique<PlayerSystemsManager>();

    m_event_manager = std::make_unique<WorldEventManager>();
}

World::~World() {
    for (auto& [key, chunk_info] : m_chunks) {
        if (chunk_info != nullptr) {
            delete chunk_info->chunk;
            delete chunk_info;
        }
    }
}

ChunkInfo* World::createChunk(int x, int z) {
    const long long chunk_index = pack_chunk_coords(x, z);

    std::lock_guard<std::mutex> lock(m_chunks_mutex);

    auto it = m_chunks.find(chunk_index);
    if (it != m_chunks.end()) {
        return it->second;
    }

    ChunkInfo* chunk_info = new ChunkInfo;
    chunk_info->x = x;
    chunk_info->z = z;
    chunk_info->chunk = new Chunk;

    m_chunks[chunk_index] = chunk_info;
    return chunk_info;
}

ChunkInfo* World::getChunkProtected(int x, int z) {
    std::lock_guard<std::mutex> lock(m_chunks_mutex);
    const long long chunk_index = pack_chunk_coords(x, z);
    auto it = m_chunks.find(chunk_index);
    if (it != m_chunks.end()) {
        return it->second;
    }
    return nullptr;
}

Chunk* World::getChunk(int x, int z, bool create) {
    ChunkInfo* chunk_info = getChunkProtected(x, z);

    if (chunk_info != nullptr) return chunk_info->chunk;

    if (!create) {
        return nullptr;
    }

    chunk_info = createChunk(x, z);
    ChunkCoord coord = { x, z };
    addEvent(
        std::make_unique<GenerateChunkEvent>(coord)
	);

    return chunk_info->chunk;
}

static inline int floorDiv(int a, int b) {
    return (a / b) - (a % b < 0 ? 1 : 0);
}

Block* World::getBlock(int world_x, int world_y, int world_z) {
    int chunk_x = floorDiv(world_x, Constants::CHUNK_SIZE_X);
    int chunk_z = floorDiv(world_z, Constants::CHUNK_SIZE_Z);

    int in_chunk_x = world_x % Constants::CHUNK_SIZE_X;
    if (in_chunk_x < 0) in_chunk_x += Constants::CHUNK_SIZE_X;

    int in_chunk_y = world_y;

    int in_chunk_z = world_z % Constants::CHUNK_SIZE_Z;
    if (in_chunk_z < 0) in_chunk_z += Constants::CHUNK_SIZE_Z;

    Chunk* chunk = getChunk(chunk_x, chunk_z);
    if (chunk == nullptr) return nullptr;

    return chunk->getBlock({ in_chunk_x, in_chunk_y, in_chunk_z });
}

void World::setBlock(int world_x, int world_y, int world_z, BlockID block_id) {
	int chunk_x = floorDiv(world_x, Constants::CHUNK_SIZE_X);
	int chunk_z = floorDiv(world_z, Constants::CHUNK_SIZE_Z);

	int in_chunk_x = world_x % Constants::CHUNK_SIZE_X;
	if (in_chunk_x < 0) in_chunk_x += Constants::CHUNK_SIZE_X;

	int in_chunk_y = world_y;

	int in_chunk_z = world_z % Constants::CHUNK_SIZE_Z;
	if (in_chunk_z < 0) in_chunk_z += Constants::CHUNK_SIZE_Z;

	ChunkInfo* chunk = getChunkProtected(chunk_x, chunk_z);
	if (chunk == nullptr) return;

	Block* block = chunk->chunk->getBlock({ in_chunk_x, in_chunk_y, in_chunk_z });
	if (block == nullptr) return;

	block->setBlockID(block_id);

	chunk->chunk->markDirty();

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

Entity World::CreatePlayer() {
    ECS* ecs = getECS();
    Entity entity = ecs->create();

    glm::ivec3 spawn_position = { 0, 0, 0 };
    ChunkCoord chunk_coord = worldToChunkCoords(spawn_position.x, spawn_position.z);

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

    generateChunks(chunk_coord.x, chunk_coord.z, 3);

    return entity;
}

ECS* World::getECS() {
    return m_ecs.ecs.get();
}

void World::tick() {
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

void World::tick_movement() {
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


Time World::getMeshGenerationTime() const {
    auto all_time = Time();
    int all_count = 0;

    for (const auto& [key, value] : m_chunks) {
        auto time = value->chunk->getChunkBuildTime();
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

void World::generateChunks(int chunk_x, int chunk_z, int radius) {
	auto generate_list = genCircleReadyA(chunk_x, chunk_z, radius);

	for (const auto& [x, z] : generate_list) {
        getChunk(x, z, true);
	}
}

ChunkCoord World::worldToChunkCoords(int world_x, int world_z) {
	return { floorDiv(world_x,  Constants::CHUNK_SIZE_X), floorDiv(world_z,  Constants::CHUNK_SIZE_X) };
}

void World::generateChunk(int x, int z) {
    Chunk* chunk = getChunk(x, z);
    m_generator->generateChunk(chunk, x, z);
}

void World::addEvent(std::unique_ptr<WorldEvent> event, bool priority)
{
    m_event_manager->push(std::move(event), priority);
}

void World::shutdown()
{
    m_event_manager->shutdown();
}
