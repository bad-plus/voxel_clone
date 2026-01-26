#pragma once

#include <core/world/block/block.h>
#include <core/world/chunk/chunk_storage.h>
#include <core/time.hpp>

#include <glm/glm.hpp>
#include <memory>

struct Chunk;

struct ChunkCoord {
	int x;
	int z;

	ChunkCoord(int x_, int z_) : x(x_), z(z_) {}
};

class Chunk {
public:
	Chunk();
	virtual ~Chunk() = default;

	int getTopBlockPosition(int x, int z);

	Block* getBlock(glm::ivec3 position);
	Block* setBlock(glm::ivec3 position, BlockID block);

	ChunkStorage* getStorage() { return m_storage.get(); }
protected:
	Block* getBlockLocal(glm::ivec3 position);

	std::unique_ptr<ChunkStorage> m_storage;
};