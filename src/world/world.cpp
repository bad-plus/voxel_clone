#include "world.h"
#include "chunk.h"
#include "block.h"
#include "../core/logger.h"
#include <GLFW/glfw3.h>
#include "generation/world_generator.h"
#include <thread>

constexpr long long pack_chunk_coords(int x, int z) {
    return ((long long)(x & 0xFFFFFFFF)) | ((long long)(z & 0xFFFFFFFF) << 32);
}

constexpr int unpack_chunk_x(long long key) {
    return (int)(key & 0xFFFFFFFF);
}

constexpr int unpack_chunk_z(long long key) {
    return (int)((key >> 32) & 0xFFFFFFFF);
}

World::World(GameContext* context, WorldGenerator* generator) {
    m_game_context = context;
    m_generator = generator;
    m_chunk_creation_time = 0.0;
}

World::~World() {
    for (auto& [key, chunk_info] : m_chunks) {
        if (chunk_info != nullptr) {
            delete chunk_info->chunk;
            delete chunk_info;
        }
    }
}

ChunkInfo* World::createChunk(int x, int z) {
    const long long chunk_index = pack_chunk_coords(x, z);

    std::lock_guard<std::mutex> lock(m_chunks_mutex);

    auto it = m_chunks.find(chunk_index);
    if (it != m_chunks.end()) {
        return it->second;
    }

    ChunkInfo* chunk_info = new ChunkInfo;
    chunk_info->x = x;
    chunk_info->z = z;
    chunk_info->chunk = new Chunk;

    m_chunks[chunk_index] = chunk_info;

    pushGenerationQueue(chunk_info);
    return chunk_info;
}
ChunkInfo* World::getChunkProtected(int x, int z) {
    std::lock_guard<std::mutex> lock(m_chunks_mutex);
    const long long chunk_index = pack_chunk_coords(x, z);
    auto it = m_chunks.find(chunk_index);
    if (it != m_chunks.end()) {
        return it->second;
    }
    return nullptr;
}

Chunk* World::getChunk(int x, int z, bool create) {
    ChunkInfo* chunk_info = getChunkProtected(x, z);

    if (chunk_info != nullptr) return chunk_info->chunk;

    if (!create) {
        return nullptr;
    }

    chunk_info = createChunk(x, z);
    return chunk_info->chunk;
}

void World::pushGenerationQueue(ChunkInfo* chunk_info) {
    if (chunk_info == nullptr) return;

    std::lock_guard<std::mutex> lock(m_generation_queue_mtx);

    for (const auto& iter : m_generation_queue) {
        if (iter->x == chunk_info->x && iter->z == chunk_info->z) return;
    }

    m_generation_queue.push_back(chunk_info);
}

ChunkInfo* World::pullGenerationQueue() {
    std::lock_guard<std::mutex> lock(m_generation_queue_mtx);
    if (m_generation_queue.empty()) return nullptr;

    ChunkInfo* chunk_info = m_generation_queue.front();
    m_generation_queue.pop_front();
    return chunk_info;
}

void World::pushToChunks(ChunkInfo* chunk_info) {
    if (chunk_info == nullptr) return;
    pushUpdateMeshQueue(chunk_info);
}

void World::processGenerationQueue() {
    ChunkInfo* chunk_info = pullGenerationQueue();
    if (chunk_info == nullptr) return;

    int x = chunk_info->x;
    int z = chunk_info->z;


    m_generator->generateChunk(chunk_info->chunk, x, z);
    pushToChunks(chunk_info);

    {
        std::lock_guard<std::mutex> lock(m_chunks_mutex);

        auto it_xm = m_chunks.find(pack_chunk_coords(x - 1, z));
        auto it_zm = m_chunks.find(pack_chunk_coords(x, z - 1));
        auto it_xp = m_chunks.find(pack_chunk_coords(x + 1, z));
        auto it_zp = m_chunks.find(pack_chunk_coords(x, z + 1));

        if (it_xm != m_chunks.end()) {
            ChunkInfo* x_m = it_xm->second;
            x_m->chunk->updateNeighbors(chunk_info->chunk, nullptr, nullptr, nullptr);
            chunk_info->chunk->updateNeighbors(nullptr, nullptr, x_m->chunk, nullptr);
            pushUpdateMeshQueue(x_m);
        }

        if (it_zm != m_chunks.end()) {
            ChunkInfo* z_m = it_zm->second;
            z_m->chunk->updateNeighbors(nullptr, chunk_info->chunk, nullptr, nullptr);
            chunk_info->chunk->updateNeighbors(nullptr, nullptr, nullptr, z_m->chunk);
            pushUpdateMeshQueue(z_m);
        }

        if (it_xp != m_chunks.end()) {
            ChunkInfo* x_p = it_xp->second;
            x_p->chunk->updateNeighbors(nullptr, nullptr, chunk_info->chunk, nullptr);
            chunk_info->chunk->updateNeighbors(x_p->chunk, nullptr, nullptr, nullptr);
            pushUpdateMeshQueue(x_p);
        }

        if (it_zp != m_chunks.end()) {
            ChunkInfo* z_p = it_zp->second;
            z_p->chunk->updateNeighbors(nullptr, nullptr, nullptr, chunk_info->chunk);
            chunk_info->chunk->updateNeighbors(nullptr, z_p->chunk, nullptr, nullptr);
            pushUpdateMeshQueue(z_p);
        }
    }
}

void World::pushUpdateMeshQueue(ChunkInfo* chunk_info) {
    if (chunk_info == nullptr) return;

    std::lock_guard<std::mutex> lock(m_update_mesh_queue_mutex);

    for (const auto& iter : m_update_mesh_queue) {
        if (iter->x == chunk_info->x && iter->z == chunk_info->z) return;
    }

    m_update_mesh_queue.push_back(chunk_info);
    chunk_info->chunk->markDirty();
}

ChunkInfo* World::pullUpdateMeshQueue() {
    std::lock_guard<std::mutex> lock(m_update_mesh_queue_mutex);
    if (m_update_mesh_queue.empty()) return nullptr;

    ChunkInfo* chunk_info = m_update_mesh_queue.front();
    m_update_mesh_queue.pop_front();
    return chunk_info;
}

void World::processUpdateMeshQueue() {
    ChunkInfo* chunk_info = pullUpdateMeshQueue();
    if (chunk_info == nullptr) return;

    chunk_info->chunk->calculateMesh();
}

inline int floorDiv(int a, int b) {
    return (a / b) - (a % b < 0 ? 1 : 0);
}

Block* World::getBlock(int world_x, int world_y, int world_z) {
    int chunk_x = floorDiv(world_x, CHUNK_SIZE_X);
    int chunk_z = floorDiv(world_z, CHUNK_SIZE_Z);

    int in_chunk_x = world_x % CHUNK_SIZE_X;
    if (in_chunk_x < 0) in_chunk_x += CHUNK_SIZE_X;

    int in_chunk_y = world_y;

    int in_chunk_z = world_z % CHUNK_SIZE_Z;
    if (in_chunk_z < 0) in_chunk_z += CHUNK_SIZE_Z;

    Chunk* chunk = getChunk(chunk_x, chunk_z);
    if (chunk == nullptr) return nullptr;

    return chunk->getBlock({ in_chunk_x, in_chunk_y, in_chunk_z });
}

void World::setBlock(int world_x, int world_y, int world_z, BlockID block_id) {
    int chunk_x = floorDiv(world_x, CHUNK_SIZE_X);
    int chunk_z = floorDiv(world_z, CHUNK_SIZE_Z);

    int in_chunk_x = world_x % CHUNK_SIZE_X;
    if (in_chunk_x < 0) in_chunk_x += CHUNK_SIZE_X;

    int in_chunk_y = world_y;

    int in_chunk_z = world_z % CHUNK_SIZE_Z;
    if (in_chunk_z < 0) in_chunk_z += CHUNK_SIZE_Z;

    ChunkInfo* chunk = getChunkProtected(chunk_x, chunk_z);
    if (chunk == nullptr) return;

    Block* block = chunk->chunk->getBlock({ in_chunk_x, in_chunk_y, in_chunk_z });
    if (block == nullptr) return;

    block->setBlockID(block_id);

    std::lock_guard<std::mutex> lock(m_chunks_mutex);

    if (in_chunk_x == 0) {
        auto it = m_chunks.find(pack_chunk_coords(chunk_x - 1, chunk_z));
        if (it != m_chunks.end()) {
            pushUpdateMeshQueue(it->second);
        }
    }

    if (in_chunk_x == CHUNK_SIZE_X - 1) {
        auto it = m_chunks.find(pack_chunk_coords(chunk_x + 1, chunk_z));
        if (it != m_chunks.end()) {
            pushUpdateMeshQueue(it->second);
        }
    }

    if (in_chunk_z == 0) {
        auto it = m_chunks.find(pack_chunk_coords(chunk_x, chunk_z - 1));
        if (it != m_chunks.end()) {
            pushUpdateMeshQueue(it->second);
        }
    }

    if (in_chunk_z == CHUNK_SIZE_Z - 1) {
        auto it = m_chunks.find(pack_chunk_coords(chunk_x, chunk_z + 1));
        if (it != m_chunks.end()) {
            pushUpdateMeshQueue(it->second);
        }
    }

    pushUpdateMeshQueue(chunk);
}