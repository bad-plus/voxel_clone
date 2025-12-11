#include "chunk.h"
#include "../core/logger.h"

#include <GLFW/glfw3.h>
#include <vector>

struct NeighborsSnapshot {
    std::vector < BlockID > x_p;
    std::vector < BlockID > z_p;
    std::vector < BlockID > x_m;
    std::vector < BlockID > z_m;
};

static unsigned int toBlocksIndex(glm::ivec3 position) {
    return position.x + position.z * CHUNK_SIZE_X + position.y * CHUNK_SIZE_X * CHUNK_SIZE_Z;
}

static glm::ivec3 toBlocksPos(const unsigned int index) {
    return {
        index % CHUNK_SIZE_X,                      // x
        index / (CHUNK_SIZE_X * CHUNK_SIZE_Z),     // y
        (index / CHUNK_SIZE_X) % CHUNK_SIZE_Z      // z
    };
}

Chunk::Chunk() {
    m_neighbors.x_p.store(nullptr);
    m_neighbors.z_p.store(nullptr);
    m_neighbors.x_m.store(nullptr);
    m_neighbors.z_m.store(nullptr);
    m_dirty = true;

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    m_blocks = new Block[CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z];

    m_indices_count = 0;

    m_ready_gpu.ready = false;
}

Chunk::~Chunk() {
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteBuffers(1, &m_EBO);

    delete[] m_blocks;
}

void Chunk::markDirty() {
    std::lock_guard<std::mutex> lock(m_dirty_mutex);
    m_dirty = true;
}

Block* Chunk::getBlock(glm::ivec3 position) {
    if (position.y < 0 || position.y >= CHUNK_SIZE_Y) return nullptr;

    if (position.x < 0) {
        Chunk* neighbor = m_neighbors.x_m.load();
        if (neighbor == nullptr) return nullptr;
        return neighbor->getBlock({ CHUNK_SIZE_X + position.x, position.y, position.z });
    }
    if (position.z < 0) {
        Chunk* neighbor = m_neighbors.z_m.load();
        if (neighbor == nullptr) return nullptr;
        return neighbor->getBlock({ position.x, position.y, CHUNK_SIZE_Z + position.z });
    }
    if (position.x >= CHUNK_SIZE_X) {
        Chunk* neighbor = m_neighbors.x_p.load();
        if (neighbor == nullptr) return nullptr;
        return neighbor->getBlock({ position.x - CHUNK_SIZE_X, position.y, position.z });
    }
    if (position.z >= CHUNK_SIZE_Z) {
        Chunk* neighbor = m_neighbors.z_p.load();
        if (neighbor == nullptr) return nullptr;
        return neighbor->getBlock({ position.x, position.y, position.z - CHUNK_SIZE_Z });
    }

    return &m_blocks[toBlocksIndex(position)];
}

Block* Chunk::getBlockLocal(glm::ivec3 position) {
    if (position.x < 0 || position.x >= CHUNK_SIZE_X ||
        position.y < 0 || position.y >= CHUNK_SIZE_Y ||
        position.z < 0 || position.z >= CHUNK_SIZE_Z) {
        return nullptr;
    }
    return &m_blocks[toBlocksIndex(position)];
}

Block* Chunk::setBlock(glm::ivec3 position, BlockID block, bool mark) {
    if (position.x < 0 || position.y < 0 || position.z < 0) return nullptr;
    if (position.x >= CHUNK_SIZE_X || position.y >= CHUNK_SIZE_Y || position.z >= CHUNK_SIZE_Z) return nullptr;
    unsigned int index = toBlocksIndex(position);
    m_blocks[index].setBlockID(block);
    if (mark) markDirty();
    return &m_blocks[index];
}

static bool checkBlockTransparent(Block* block) {
    if (block == nullptr) return true;
    return BlocksInfo[block->getBlockID()].isTransparent;
}

static bool checkBlockTransparentSafe(const std::vector<BlockID>& snapshot, glm::ivec3 pos, NeighborsSnapshot& neighbors) {
    if (pos.y < 0 || pos.y >= CHUNK_SIZE_Y) return true;

    if (pos.x < 0) {
        if (neighbors.x_m.empty()) return true;
        
        return BlocksInfo[neighbors.x_m[toBlocksIndex({ CHUNK_SIZE_X + pos.x, pos.y, pos.z })]].isTransparent;
    }

    if (pos.z < 0) {
        if (neighbors.z_m.empty()) return true;

        return BlocksInfo[neighbors.z_m[toBlocksIndex({ pos.x, pos.y, CHUNK_SIZE_Z + pos.z })]].isTransparent;
    }
    
    if (pos.x >= CHUNK_SIZE_X) {
        if (neighbors.x_p.empty()) return true;

        return BlocksInfo[neighbors.x_p[toBlocksIndex({ pos.x - CHUNK_SIZE_X, pos.y, pos.z })]].isTransparent;
    }

    if (pos.z >= CHUNK_SIZE_Z) {
        if (neighbors.z_p.empty()) return true;

        return BlocksInfo[neighbors.z_p[toBlocksIndex({ pos.x, pos.y, pos.z - CHUNK_SIZE_Z })]].isTransparent;
    }

    size_t idx = toBlocksIndex(pos);
    return BlocksInfo[snapshot[idx]].isTransparent;
}

std::vector < BlockID > Chunk::getBlocksSnapshot() {
    std::vector<BlockID> snapshot;
    snapshot.reserve(CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z);

    {
        std::lock_guard<std::mutex> lock(m_dirty_mutex);

        for (size_t i = 0; i < CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z; i++) {
            snapshot.push_back(m_blocks[i].getBlockID());
        }
    }

    return snapshot;
}

void Chunk::calculateMesh() {
    if (!m_dirty) return;

    std::vector<BlockID> block_snapshot = getBlocksSnapshot();
    NeighborsSnapshot neighbors;
    if (m_neighbors.x_p != nullptr) neighbors.x_p = m_neighbors.x_p.load()->getBlocksSnapshot();
    if (m_neighbors.z_p != nullptr) neighbors.z_p = m_neighbors.z_p.load()->getBlocksSnapshot();
    if (m_neighbors.x_m != nullptr) neighbors.x_m = m_neighbors.x_m.load()->getBlocksSnapshot();
    if (m_neighbors.z_m != nullptr) neighbors.z_m = m_neighbors.z_m.load()->getBlocksSnapshot();

    std::vector<GLfloat> temp_vertices;
    std::vector<GLuint> temp_indices;

    constexpr int VERTICES_BUFFER_SIZE = CHUNK_SIZE_X * CHUNK_SIZE_Z * 64 * 4;

    temp_vertices.reserve(VERTICES_BUFFER_SIZE);
    temp_indices.reserve(VERTICES_BUFFER_SIZE);

    for (unsigned int x = 0; x < CHUNK_SIZE_X; x++) {
        for (unsigned int y = 0; y < CHUNK_SIZE_Y; y++) {
            for (unsigned int z = 0; z < CHUNK_SIZE_Z; z++) {
                size_t idx = toBlocksIndex({ x, y, z });
                BlockID block_id = block_snapshot[idx];

                if (block_id == BlockID::EMPTY) continue;

                BlockTexture* block_texture = &BlocksInfo[block_id].texture;

                // TOP (Y+)
                if (checkBlockTransparentSafe(block_snapshot, { (int)x, (int)y + 1, (int)z }, neighbors)) {
                    SideUV face_uv = block_texture->sides[(unsigned int)BlockSide::TOP];
                    GLuint vertex_offset = temp_vertices.size() / 8;

                    GLfloat face_vertices[] = {
                        x + 0.0f, y + 1.0f, z + 1.0f, 0.0f, 1.0f, 0.0f, face_uv.uv[0].x, face_uv.uv[0].y,
                        x + 1.0f, y + 1.0f, z + 1.0f, 0.0f, 1.0f, 0.0f, face_uv.uv[1].x, face_uv.uv[1].y,
                        x + 1.0f, y + 1.0f, z + 0.0f, 0.0f, 1.0f, 0.0f, face_uv.uv[2].x, face_uv.uv[2].y,
                        x + 0.0f, y + 1.0f, z + 0.0f, 0.0f, 1.0f, 0.0f, face_uv.uv[3].x, face_uv.uv[3].y,
                    };

                    GLuint face_indices[] = {
                        vertex_offset + 0, vertex_offset + 1, vertex_offset + 2,
                        vertex_offset + 2, vertex_offset + 3, vertex_offset + 0
                    };

                    temp_vertices.insert(temp_vertices.end(), std::begin(face_vertices), std::end(face_vertices));
                    temp_indices.insert(temp_indices.end(), std::begin(face_indices), std::end(face_indices));
                }

                // BOTTOM (Y-)
                if (checkBlockTransparentSafe(block_snapshot, { (int)x, (int)y - 1, (int)z }, neighbors)) {
                    SideUV face_uv = block_texture->sides[(unsigned int)BlockSide::BOTTOM];
                    GLuint vertex_offset = temp_vertices.size() / 8;

                    GLfloat face_vertices[] = {
                        x + 0.0f, y + 0.0f, z + 0.0f, 0.0f, -1.0f, 0.0f, face_uv.uv[0].x, face_uv.uv[0].y,
                        x + 1.0f, y + 0.0f, z + 0.0f, 0.0f, -1.0f, 0.0f, face_uv.uv[1].x, face_uv.uv[1].y,
                        x + 1.0f, y + 0.0f, z + 1.0f, 0.0f, -1.0f, 0.0f, face_uv.uv[2].x, face_uv.uv[2].y,
                        x + 0.0f, y + 0.0f, z + 1.0f, 0.0f, -1.0f, 0.0f, face_uv.uv[3].x, face_uv.uv[3].y,
                    };

                    GLuint face_indices[] = {
                        vertex_offset + 0, vertex_offset + 1, vertex_offset + 2,
                        vertex_offset + 2, vertex_offset + 3, vertex_offset + 0
                    };

                    temp_vertices.insert(temp_vertices.end(), std::begin(face_vertices), std::end(face_vertices));
                    temp_indices.insert(temp_indices.end(), std::begin(face_indices), std::end(face_indices));
                }

                // FRONT (Z+)
                if (checkBlockTransparentSafe(block_snapshot, { (int)x, (int)y, (int)z + 1 }, neighbors)) {
                    SideUV face_uv = block_texture->sides[(unsigned int)BlockSide::FRONT];
                    GLuint vertex_offset = temp_vertices.size() / 8;

                    GLfloat face_vertices[] = {
                        x + 0.0f, y + 0.0f, z + 1.0f, 0.0f, 0.0f, 1.0f, face_uv.uv[0].x, face_uv.uv[0].y,
                        x + 1.0f, y + 0.0f, z + 1.0f, 0.0f, 0.0f, 1.0f, face_uv.uv[1].x, face_uv.uv[1].y,
                        x + 1.0f, y + 1.0f, z + 1.0f, 0.0f, 0.0f, 1.0f, face_uv.uv[2].x, face_uv.uv[2].y,
                        x + 0.0f, y + 1.0f, z + 1.0f, 0.0f, 0.0f, 1.0f, face_uv.uv[3].x, face_uv.uv[3].y,
                    };

                    GLuint face_indices[] = {
                        vertex_offset + 0, vertex_offset + 1, vertex_offset + 2,
                        vertex_offset + 2, vertex_offset + 3, vertex_offset + 0
                    };

                    temp_vertices.insert(temp_vertices.end(), std::begin(face_vertices), std::end(face_vertices));
                    temp_indices.insert(temp_indices.end(), std::begin(face_indices), std::end(face_indices));
                }

                // BACK (Z-)
                if (checkBlockTransparentSafe(block_snapshot, { (int)x, (int)y, (int)z - 1 }, neighbors)) {
                    SideUV face_uv = block_texture->sides[(unsigned int)BlockSide::BACK];
                    GLuint vertex_offset = temp_vertices.size() / 8;

                    GLfloat face_vertices[] = {
                        x + 1.0f, y + 0.0f, z + 0.0f, 0.0f, 0.0f, -1.0f, face_uv.uv[0].x, face_uv.uv[0].y,
                        x + 0.0f, y + 0.0f, z + 0.0f, 0.0f, 0.0f, -1.0f, face_uv.uv[1].x, face_uv.uv[1].y,
                        x + 0.0f, y + 1.0f, z + 0.0f, 0.0f, 0.0f, -1.0f, face_uv.uv[2].x, face_uv.uv[2].y,
                        x + 1.0f, y + 1.0f, z + 0.0f, 0.0f, 0.0f, -1.0f, face_uv.uv[3].x, face_uv.uv[3].y,
                    };

                    GLuint face_indices[] = {
                        vertex_offset + 0, vertex_offset + 1, vertex_offset + 2,
                        vertex_offset + 2, vertex_offset + 3, vertex_offset + 0
                    };

                    temp_vertices.insert(temp_vertices.end(), std::begin(face_vertices), std::end(face_vertices));
                    temp_indices.insert(temp_indices.end(), std::begin(face_indices), std::end(face_indices));
                }

                // RIGHT (X+)
                if (checkBlockTransparentSafe(block_snapshot, { (int)x + 1, (int)y, (int)z }, neighbors)) {
                    SideUV face_uv = block_texture->sides[(unsigned int)BlockSide::RIGHT];
                    GLuint vertex_offset = temp_vertices.size() / 8;

                    GLfloat face_vertices[] = {
                        x + 1.0f, y + 0.0f, z + 1.0f, 1.0f, 0.0f, 0.0f, face_uv.uv[0].x, face_uv.uv[0].y,
                        x + 1.0f, y + 0.0f, z + 0.0f, 1.0f, 0.0f, 0.0f, face_uv.uv[1].x, face_uv.uv[1].y,
                        x + 1.0f, y + 1.0f, z + 0.0f, 1.0f, 0.0f, 0.0f, face_uv.uv[2].x, face_uv.uv[2].y,
                        x + 1.0f, y + 1.0f, z + 1.0f, 1.0f, 0.0f, 0.0f, face_uv.uv[3].x, face_uv.uv[3].y,
                    };

                    GLuint face_indices[] = {
                        vertex_offset + 0, vertex_offset + 1, vertex_offset + 2,
                        vertex_offset + 2, vertex_offset + 3, vertex_offset + 0
                    };

                    temp_vertices.insert(temp_vertices.end(), std::begin(face_vertices), std::end(face_vertices));
                    temp_indices.insert(temp_indices.end(), std::begin(face_indices), std::end(face_indices));
                }

                // LEFT (X-)
                if (checkBlockTransparentSafe(block_snapshot, { (int)x - 1, (int)y, (int)z }, neighbors)) {
                    SideUV face_uv = block_texture->sides[(unsigned int)BlockSide::LEFT];
                    GLuint vertex_offset = temp_vertices.size() / 8;

                    GLfloat face_vertices[] = {
                        x + 0.0f, y + 0.0f, z + 0.0f, -1.0f, 0.0f, 0.0f, face_uv.uv[0].x, face_uv.uv[0].y,
                        x + 0.0f, y + 0.0f, z + 1.0f, -1.0f, 0.0f, 0.0f, face_uv.uv[1].x, face_uv.uv[1].y,
                        x + 0.0f, y + 1.0f, z + 1.0f, -1.0f, 0.0f, 0.0f, face_uv.uv[2].x, face_uv.uv[2].y,
                        x + 0.0f, y + 1.0f, z + 0.0f, -1.0f, 0.0f, 0.0f, face_uv.uv[3].x, face_uv.uv[3].y,
                    };

                    GLuint face_indices[] = {
                        vertex_offset + 0, vertex_offset + 1, vertex_offset + 2,
                        vertex_offset + 2, vertex_offset + 3, vertex_offset + 0
                    };

                    temp_vertices.insert(temp_vertices.end(), std::begin(face_vertices), std::end(face_vertices));
                    temp_indices.insert(temp_indices.end(), std::begin(face_indices), std::end(face_indices));
                }
            }
        }
    }

    {
        std::lock_guard<std::mutex> lock(m_dirty_mutex);
        m_ready_gpu.vertices = std::move(temp_vertices);
        m_ready_gpu.indices = std::move(temp_indices);
        m_ready_gpu.ready = true;
        m_dirty = false;
    }
}

void Chunk::uploadMeshToGPU() {
    if (!m_ready_gpu.ready) return;
    if (m_ready_gpu.vertices.size() == 0 || m_ready_gpu.indices.size() == 0) return;
    
    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, m_ready_gpu.vertices.size() * sizeof(GLfloat), m_ready_gpu.vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_ready_gpu.indices.size() * sizeof(GLuint), m_ready_gpu.indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);

    m_indices_count = m_ready_gpu.indices.size();
    m_ready_gpu.ready = false;
}

void Chunk::draw() {
    if (m_ready_gpu.ready) uploadMeshToGPU();
    if (m_indices_count == 0) return;

    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, m_indices_count, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Chunk::updateNeighbors(Chunk* x_p, Chunk* z_p, Chunk* x_m, Chunk* z_m) {
    if (x_p != nullptr) m_neighbors.x_p.store(x_p);
    if (z_p != nullptr) m_neighbors.z_p.store(z_p);
    if (x_m != nullptr) m_neighbors.x_m.store(x_m);
    if (z_m != nullptr) m_neighbors.z_m.store(z_m);
}

bool Chunk::isDirty() {
    std::lock_guard<std::mutex> lock(m_dirty_mutex);
    return m_dirty;
}

int Chunk::getTopBlockPosition(int x, int z) {
    for (int y = CHUNK_SIZE_Y - 1; y >= 0; y--) {
        Block* block = getBlockLocal({ x, y, z });

        if (block->getBlockID() != BlockID::EMPTY) {
            return y;
        }
    }
    return 0;
}