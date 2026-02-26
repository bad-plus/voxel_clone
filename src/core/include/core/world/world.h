#pragma once
#include <unordered_map>
#include <deque>
#include <mutex>
#include <memory>
#include <core/world/block/block.h>
#include <core/ecs/core/entity.h>
#include <core/world/chunk/chunk.h>
#include <core/time.hpp>

class World {
public:
	World();
	virtual ~World();

	Chunk* getChunk(int x, int z);
	Block* getBlock(int world_x, int world_y, int world_z);
	virtual void setBlock(int world_x, int world_y, int world_z, BlockID block_id);

	virtual Chunk* createChunk(int x, int z);

	virtual void shutdown();
	
	void loadFromBytes(ChunkCoord position, const std::vector<uint8_t>& bytes);
protected:
	std::unordered_map<long long, Chunk*> m_chunks;
	std::mutex m_chunks_mutex;
};