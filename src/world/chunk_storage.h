#pragma once
#include "block.h"

#include <array>

constexpr unsigned int CHUNK_SIZE_X = 16;
constexpr unsigned int CHUNK_SIZE_Y = 512; // height
constexpr unsigned int CHUNK_SIZE_Z = 16;

class ChunkStorage {
public:
	ChunkStorage();
	~ChunkStorage();

	Block* getBlock(glm::ivec3 position);
	bool setBlock(glm::ivec3 position, BlockID block_id);

	Block* getBlockUnsafe(glm::ivec3 position);
	void setBlockUnsafe(glm::ivec3 position, BlockID block_id);

	static constexpr int getSizeX() { return CHUNK_SIZE_X; }
	static constexpr int getSizeY() { return CHUNK_SIZE_Y; }
	static constexpr int getSizeZ() { return CHUNK_SIZE_Z; }

	int getTopBlockY(int x, int z);
	bool isValidPosition(glm::ivec3 position) const;

	static size_t toIndex(int x, int y, int z) {
		return x + z * CHUNK_SIZE_X + y * CHUNK_SIZE_X * CHUNK_SIZE_Z;
	}

	static size_t toIndex(glm::ivec3 position) {
		return toIndex(position.x, position.y, position.z);
	}

	static glm::ivec3 toBlockPos(const unsigned int index) {
		return {
			index % CHUNK_SIZE_X,                      // x
			index / (CHUNK_SIZE_X * CHUNK_SIZE_Z),     // y
			(index / CHUNK_SIZE_X) % CHUNK_SIZE_Z      // z
		};
	}
private:
	std::array<Block, CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z> m_blocks;
};