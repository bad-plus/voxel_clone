#pragma once

#include <core/world/block/block.h>
#include <core/world/chunk/chunk_storage.h>
#include <core/time.hpp>

#include <glm/glm.hpp>
#include <memory>

struct Chunk;

static inline int floorDiv(int a, int b) {
	return (a / b) - (a % b < 0 ? 1 : 0);
}

struct ChunkCoord {
	int x;
	int z;

	ChunkCoord() : x(0), z(0) {}
	ChunkCoord(int x_, int z_) : x(x_), z(z_) {}
	ChunkCoord(long long index) :
		x((int)(index & 0xFFFFFFFF)),
		z((int)((index >> 32) & 0xFFFFFFFF))
	{
	}

	static ChunkCoord fromWorldPosition(int world_x, int world_z) {
		return ChunkCoord(
			floorDiv(world_x, Constants::CHUNK_SIZE_X), 
			floorDiv(world_z, Constants::CHUNK_SIZE_Z));
	}

	long long getIndex() const {
		return ((long long)(x & 0xFFFFFFFF)) | ((long long)(z & 0xFFFFFFFF) << 32);
	}
};

class Chunk {
public:
	Chunk();
	virtual ~Chunk() = default;

	int getTopBlockPosition(int x, int z);

	Block* getBlock(glm::ivec3 position);
	Block* setBlock(glm::ivec3 position, BlockID block);

	ChunkStorage* getStorage() { return m_storage.get(); }

	std::vector<uint8_t> to_bytes();
	void from_bytes(const std::vector<uint8_t>& bytes);
protected:
	Block* getBlockLocal(glm::ivec3 position);

	std::unique_ptr<ChunkStorage> m_storage;
};