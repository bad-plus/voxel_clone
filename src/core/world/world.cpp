#include <core/world/world.h>

World::World()
{

}

World::~World()
{

}

Chunk* World::getChunk(int x, int z)
{
    std::lock_guard<std::mutex> lock(m_chunks_mutex);
    const long long chunk_index = ChunkCoord(x, z).getIndex();
    auto it = m_chunks.find(chunk_index);
    if (it != m_chunks.end()) {
        return it->second;
    }
    return nullptr;
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

void World::setBlock(int world_x, int world_y, int world_z, BlockID block_id)
{
    int chunk_x = floorDiv(world_x, Constants::CHUNK_SIZE_X);
    int chunk_z = floorDiv(world_z, Constants::CHUNK_SIZE_Z);

    int in_chunk_x = world_x % Constants::CHUNK_SIZE_X;
    if (in_chunk_x < 0) in_chunk_x += Constants::CHUNK_SIZE_X;

    int in_chunk_y = world_y;

    int in_chunk_z = world_z % Constants::CHUNK_SIZE_Z;
    if (in_chunk_z < 0) in_chunk_z += Constants::CHUNK_SIZE_Z;

    Chunk* chunk = getChunk(chunk_x, chunk_z);
    if (chunk == nullptr) return;

    Block* block = chunk->getBlock({ in_chunk_x, in_chunk_y, in_chunk_z });
    if (block == nullptr) return;

    block->setBlockID(block_id);
}

Chunk* World::createChunk(int x, int z)
{
    const long long chunk_index = ChunkCoord(x, z).getIndex();

    std::lock_guard<std::mutex> lock(m_chunks_mutex);

    auto it = m_chunks.find(chunk_index);
    if (it != m_chunks.end()) {
        return dynamic_cast<Chunk*>(it->second);
    }

    Chunk* new_chunk = new Chunk();
    m_chunks[chunk_index] = new_chunk;
    return new_chunk;
}

void World::shutdown()
{

}

void World::loadFromBytes(ChunkCoord position, const std::vector<uint8_t>& bytes)
{
}
