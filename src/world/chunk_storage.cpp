#include "chunk_storage.h"

ChunkStorage::ChunkStorage() {
	m_blocks.fill(BlockID::EMPTY);
}

ChunkStorage::~ChunkStorage() {

}

Block* ChunkStorage::getBlock(glm::ivec3 position) {
	if (!isValidPosition(position)) return nullptr;
	return getBlockUnsafe(position);
}

bool ChunkStorage::setBlock(glm::ivec3 position, BlockID block_id) {
	if (!isValidPosition(position)) return false;
	setBlockUnsafe(position, block_id);
	return true;
}

Block* ChunkStorage::getBlockUnsafe(glm::ivec3 position) {
	size_t index = ChunkStorage::toIndex(position);

	return &m_blocks[index];
}

void ChunkStorage::setBlockUnsafe(glm::ivec3 position, BlockID block_id) {
	size_t index = ChunkStorage::toIndex(position);

	m_blocks[index].setBlockID(block_id);
}

int ChunkStorage::getTopBlockY(int x, int z) {
	for (int y = CHUNK_SIZE_Y - 1; y >= 0; y--) {
		Block* block = getBlockUnsafe({ x, y, z });

		if (block->getBlockID() != BlockID::EMPTY) {
			return y;
		}
	}
	return 0;
}

bool ChunkStorage::isValidPosition(glm::ivec3 position) const {
	if (position.x < 0 || position.x >= CHUNK_SIZE_X) return false;
	if (position.y < 0 || position.y >= CHUNK_SIZE_Y) return false;
	if (position.z < 0 || position.z >= CHUNK_SIZE_Z) return false;
	return true;
}