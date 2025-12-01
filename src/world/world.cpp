#include "world.h"
#include "chunk.h"
#include "../core/logger.h"
#include <GLFW/glfw3.h>
#include "world_generator.h"

constexpr long long pack_chunk_coords(int x, int z) {
    return ((long long)(x & 0xFFFFFFFF)) | ((long long)(z & 0xFFFFFFFF) << 32);
}

constexpr int unpack_chunk_x(long long key) {
    return (int)(key & 0xFFFFFFFF);
}

constexpr int unpack_chunk_z(long long key) {
    return (int)((key >> 32) & 0xFFFFFFFF);
}

World::World(GameContext* context, WorldGeneator* generator) {
    m_game_context = context;
    m_generator = generator;
}
World::~World() {
    for(auto& [key, chunk_info] : m_chunks) {
        if(chunk_info != nullptr) {
            delete chunk_info->chunk;
            delete chunk_info;
        }
    }
}

ChunkInfo* World::createChunk(int x, int z) {
    double start_creation_time = glfwGetTime();
    ChunkInfo* chunk_info = new ChunkInfo;
    chunk_info->x = x;
    chunk_info->z = z;
    chunk_info->chunk = new Chunk;

    chunk_info->chunk->markDirty();
    double end_creation_time = glfwGetTime();
    m_chunk_creation_time = end_creation_time - start_creation_time;
    return chunk_info;
}

Chunk* World::getChunk(int x, int z, bool create) {
    const long long chunk_index = pack_chunk_coords(x, z);

    auto it = m_chunks.find(chunk_index);
    
    if(it != m_chunks.end()) {
        return (it->second != nullptr) ? it->second->chunk : nullptr;
    }

    if(!create) {
        return nullptr;
    }
    
    ChunkInfo* chunk_info = createChunk(x, z);
    m_chunks[chunk_index] = chunk_info;
    
    Chunk* x_m = getChunk(x - 1, z, false);
    if(x_m != nullptr) {
        x_m->updateNeighbors(chunk_info->chunk, nullptr, nullptr, nullptr);
        x_m->markDirty();
        chunk_info->chunk->updateNeighbors(nullptr, nullptr, x_m, nullptr);
    }

    Chunk* z_m = getChunk(x, z - 1, false);
    if(z_m != nullptr) {
        z_m->updateNeighbors(nullptr, chunk_info->chunk, nullptr, nullptr);
        z_m->markDirty();
        chunk_info->chunk->updateNeighbors(nullptr, nullptr, nullptr, z_m);   
    }

    Chunk* x_p = getChunk(x + 1, z, false);
    if(x_p != nullptr) {
        x_p->updateNeighbors(nullptr, nullptr, chunk_info->chunk, nullptr);
        x_p->markDirty();
        chunk_info->chunk->updateNeighbors(x_p, nullptr, nullptr, nullptr);
    }

    Chunk* z_p = getChunk(x, z + 1, false);
    if(z_p != nullptr) {
        z_p->updateNeighbors(nullptr, nullptr, nullptr, chunk_info->chunk);
        z_p->markDirty();
        chunk_info->chunk->updateNeighbors(nullptr, z_p, nullptr, nullptr);
    }

    return chunk_info->chunk;
}

void World::updateMeshChunks() {
    for(auto& [key, chunk_info] : m_chunks) {
        if(chunk_info != nullptr) {
            chunk_info->chunk->updateMesh();
        }
    }
}

void World::generateChunks() {
    for(auto& [key, chunk_info] : m_chunks) {
        if(chunk_info != nullptr) {
            if(!chunk_info->chunk->isGenerated()) {
                m_generator->generateChunk(chunk_info->chunk, chunk_info->x, chunk_info->z);
            }
        }
    }
}