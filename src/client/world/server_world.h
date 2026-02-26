#pragma once
#include <unordered_map>
#include <deque>
#include <mutex>
#include <memory>
#include <core/world/block/block.h>
#include <core/ecs/core/entity.h>
#include <core/world/chunk/chunk.h>
#include "chunk/client_chunk.h"
#include "world/world_event.h"
#include <core/time.hpp>
#include <core/world/world.h>

struct WorldGenerator;

class ServerWorld : public World {
public:
	ServerWorld(WorldGenerator* generator = nullptr);
	~ServerWorld();

	void generateChunk(int x, int z);

	const WorldGenerator* getGenerator() const { return m_generator; }
private:
	WorldGenerator* m_generator;
};