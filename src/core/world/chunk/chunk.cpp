#include <core/world/chunk/chunk.h>
#include <core/logger.hpp>

#include <core/time.hpp>

Chunk::Chunk() {
	m_storage = std::make_unique<ChunkStorage>();
}

Block* Chunk::getBlock(glm::ivec3 position) {
	if (position.y < 0 || position.y >= Constants::CHUNK_SIZE_Y) return nullptr;

	if (position.x < 0) return nullptr;
	if (position.z < 0) return nullptr;
	if (position.x >= Constants::CHUNK_SIZE_X) return nullptr;
	if (position.z >= Constants::CHUNK_SIZE_Z) return nullptr;

	return m_storage->getBlock(position);
}

std::vector<uint8_t> Chunk::to_bytes()
{
	return m_storage->to_bytes();
}

void Chunk::from_bytes(const std::vector<uint8_t>& bytes)
{
	return m_storage->from_bytes(bytes);
}

Block* Chunk::getBlockLocal(glm::ivec3 position) {
	return m_storage->getBlock(position);
}

Block* Chunk::setBlock(glm::ivec3 position, BlockID block) {
	if (!m_storage->setBlock(position, block)) return nullptr;
	return m_storage->getBlock(position);
}

int Chunk::getTopBlockPosition(int x, int z) {
	return m_storage->getTopBlockY(x, z);
}