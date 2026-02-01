#include <core/world/chunk/chunk_storage.h>
#include <core/constants.h>
#include <core/logger.hpp>

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
	for (int y = Constants::CHUNK_SIZE_Y - 1; y >= 0; y--) {
		Block* block = getBlockUnsafe({ x, y, z });

		if (block->getBlockID() != BlockID::EMPTY) {
			return y;
		}
	}
	return 0;
}

bool ChunkStorage::isValidPosition(glm::ivec3 position) const {
	if (position.x < 0 || position.x >= Constants::CHUNK_SIZE_X) return false;
	if (position.y < 0 || position.y >= Constants::CHUNK_SIZE_Y) return false;
	if (position.z < 0 || position.z >= Constants::CHUNK_SIZE_Z) return false;
	return true;
}

std::vector<uint8_t> ChunkStorage::to_bytes()
{
	std::vector<uint8_t> result;
	for (size_t i = 0; i < Constants::CHUNK_SIZE_VOLUME; i++) {
		auto block_data = m_blocks[i].to_bytes();
		result.insert(result.end(), block_data.begin(), block_data.end());
	}
	return result;
}

void ChunkStorage::from_bytes(const std::vector<uint8_t>& bytes)
{
	constexpr size_t BLOCK_SIZE = 2; 
	constexpr size_t EXPECTED_SIZE = Constants::CHUNK_SIZE_VOLUME * BLOCK_SIZE;

	if (bytes.size() < EXPECTED_SIZE) {
		LOG_ERROR("Invalid chunk data size: {} (expected {})", bytes.size(), EXPECTED_SIZE);
		return;
	}

	size_t pos = 0;
	for (size_t i = 0; i < Constants::CHUNK_SIZE_VOLUME; i++) {
		std::vector<uint8_t> block_data(bytes.begin() + pos, bytes.begin() + pos + BLOCK_SIZE);
		m_blocks[i].from_bytes(block_data);
		pos += BLOCK_SIZE;
	}
}
