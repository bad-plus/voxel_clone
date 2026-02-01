#pragma once
#include <core/world/block/block.h>
#include <core/constants.h>

#include <array>

class ChunkStorage {
public:
	ChunkStorage();
	~ChunkStorage();

	Block* getBlock(glm::ivec3 position);
	bool setBlock(glm::ivec3 position, BlockID block_id);

	Block* getBlockUnsafe(glm::ivec3 position);
	void setBlockUnsafe(glm::ivec3 position, BlockID block_id);

	int getTopBlockY(int x, int z);
	bool isValidPosition(glm::ivec3 position) const;

	static size_t toIndex(int x, int y, int z) {
		return x + z * Constants::CHUNK_SIZE_X + y * Constants::CHUNK_SIZE_X * Constants::CHUNK_SIZE_Z;
	}

	static size_t toIndex(glm::ivec3 position) {
		return toIndex(position.x, position.y, position.z);
	}

	static glm::ivec3 toBlockPos(const unsigned int index) {
		return {
			index % Constants::CHUNK_SIZE_X,								// x
			index / (Constants::CHUNK_SIZE_X * Constants::CHUNK_SIZE_Z),    // y
			(index / Constants::CHUNK_SIZE_X) % Constants::CHUNK_SIZE_Z     // z
		};
	}

	std::vector<uint8_t> to_bytes();
	void from_bytes(const std::vector<uint8_t>& bytes);
private:
	std::array<Block, Constants::CHUNK_SIZE_VOLUME> m_blocks;
};