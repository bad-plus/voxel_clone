#include "chunk.h"
#include "../core/logger.h"

#include <GLFW/glfw3.h>
#include <vector>

unsigned int toBlocksIndex(glm::ivec3 position) {
    return position.x + position.z * CHUNK_SIZE_X + position.y * CHUNK_SIZE_X * CHUNK_SIZE_Z;
}

glm::ivec3 toBlocksPos(unsigned int index) {
    return {
        index % CHUNK_SIZE_X,                      // x
        index / (CHUNK_SIZE_X * CHUNK_SIZE_Z),     // y
        (index / CHUNK_SIZE_X) % CHUNK_SIZE_Z      // z
    };
}

Chunk::Chunk() {
    m_neighbors = {nullptr, nullptr, nullptr, nullptr};
    m_dirty = true;

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    m_blocks = new Block[CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z];

    updateMesh();
}

Chunk::~Chunk() {
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteBuffers(1, &m_EBO);

    delete[] m_blocks;
}

void Chunk::markDirty() {
    m_dirty = true;
}

Block* Chunk::getBlock(glm::ivec3 position) {
    if(position.y < 0 || position.y >= CHUNK_SIZE_Y) return nullptr;
    
    if(position.x < 0) {
        if(m_neighbors.x_m == nullptr) return nullptr;
        return m_neighbors.x_m->getBlock({CHUNK_SIZE_X + position.x, position.y, position.z});
    }
    if(position.z < 0) {
        if(m_neighbors.z_m == nullptr) return nullptr;
        return m_neighbors.z_m->getBlock({position.x, position.y, CHUNK_SIZE_Z + position.z});
    }
    if(position.x >= CHUNK_SIZE_X) {
        if(m_neighbors.x_p == nullptr) return nullptr;
        return m_neighbors.x_p->getBlock({position.x - CHUNK_SIZE_X, position.y, position.z});
    }
    if(position.z >= CHUNK_SIZE_Z) {
        if(m_neighbors.z_p == nullptr) return nullptr;
        return m_neighbors.z_p->getBlock({position.x, position.y, position.z - CHUNK_SIZE_Z});
    }
    
    return &m_blocks[toBlocksIndex(position)];
}

Block* Chunk::setBlock(glm::ivec3 position, BlockID block, bool mark) {
    unsigned int index = toBlocksIndex(position);
    m_blocks[index].setBlockID(block);
    if(mark) markDirty();
    return &m_blocks[index];
}

bool checkBlockTransparent(Block* block) {
    if(block == nullptr) return true;
    
    return BlocksInfo[block->getBlockID()].isTransparent;
}

void Chunk::updateMesh() {
    if(!m_dirty) return;

    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;

    for(unsigned int x = 0; x < CHUNK_SIZE_X; x++) {
        for(unsigned int y = 0; y < CHUNK_SIZE_Y; y++) {
            for(unsigned int z = 0; z < CHUNK_SIZE_Z; z++) {
                Block* block = getBlock({x, y, z});
                
                if(block == nullptr || block->getBlockID() == BlockID::EMPTY) continue;

                BlockTexture* block_texture = &BlocksInfo[block->getBlockID()].texture;

                // TOP (Y+) - Смотрит ВВЕРХ
                if(checkBlockTransparent(getBlock({x, y + 1, z}))) {
                    SideUV face_uv = block_texture->sides[(unsigned int)BlockSide::TOP];
                    GLuint vertex_offset = vertices.size() / 8;

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

                    vertices.insert(vertices.end(), std::begin(face_vertices), std::end(face_vertices));
                    indices.insert(indices.end(), std::begin(face_indices), std::end(face_indices));
                }

                //
                if(checkBlockTransparent(getBlock({x, y - 1, z}))) {
                    SideUV face_uv = block_texture->sides[(unsigned int)BlockSide::BOTTOM];
                    GLuint vertex_offset = vertices.size() / 8;

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

                    vertices.insert(vertices.end(), std::begin(face_vertices), std::end(face_vertices));
                    indices.insert(indices.end(), std::begin(face_indices), std::end(face_indices));
                }

                // FRONT (Z+) - Смотрит в направлении +Z
                if(checkBlockTransparent(getBlock({x, y, z + 1}))) {
                    SideUV face_uv = block_texture->sides[(unsigned int)BlockSide::FRONT];
                    GLuint vertex_offset = vertices.size() / 8;

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

                    vertices.insert(vertices.end(), std::begin(face_vertices), std::end(face_vertices));
                    indices.insert(indices.end(), std::begin(face_indices), std::end(face_indices));
                }

                // BACK (Z-) - Смотрит в направлении -Z
                if(checkBlockTransparent(getBlock({x, y, z - 1}))) {
                    SideUV face_uv = block_texture->sides[(unsigned int)BlockSide::BACK];
                    GLuint vertex_offset = vertices.size() / 8;

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

                    vertices.insert(vertices.end(), std::begin(face_vertices), std::end(face_vertices));
                    indices.insert(indices.end(), std::begin(face_indices), std::end(face_indices));
                }

                // RIGHT (X+) - Смотрит в направлении +X
                if(checkBlockTransparent(getBlock({x + 1, y, z}))) {
                    SideUV face_uv = block_texture->sides[(unsigned int)BlockSide::RIGHT];
                    GLuint vertex_offset = vertices.size() / 8;

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

                    vertices.insert(vertices.end(), std::begin(face_vertices), std::end(face_vertices));
                    indices.insert(indices.end(), std::begin(face_indices), std::end(face_indices));
                }

                // LEFT (X-) - Смотрит в направлении -X
                if(checkBlockTransparent(getBlock({x - 1, y, z}))) {
                    SideUV face_uv = block_texture->sides[(unsigned int)BlockSide::LEFT];
                    GLuint vertex_offset = vertices.size() / 8;

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

                    vertices.insert(vertices.end(), std::begin(face_vertices), std::end(face_vertices));
                    indices.insert(indices.end(), std::begin(face_indices), std::end(face_indices));
                }
            }
        }
    }

    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);

    m_indices_count = indices.size();
    m_dirty = false;
}



void Chunk::draw() {
    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, m_indices_count, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Chunk::updateNeighbors(Chunk* x_p, Chunk* z_p, Chunk* x_m, Chunk* z_m) {
    if(x_p != nullptr) m_neighbors.x_p = x_p;
    if(z_p != nullptr) m_neighbors.z_p = z_p;
    if(x_m != nullptr) m_neighbors.x_m = x_m;
    if(z_m != nullptr) m_neighbors.z_m = z_m;
}

bool Chunk::isDirty() {
    return m_dirty;
}