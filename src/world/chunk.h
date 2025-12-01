#pragma once

#include "block.h"

#include <glm/glm.hpp>
#include <glad/glad.h>

const unsigned int CHUNK_SIZE_X =        16;
const unsigned int CHUNK_SIZE_Y =        256; // height
const unsigned int CHUNK_SIZE_Z =        16;

struct Chunk;

class Chunk {
public:
    Chunk();
    ~Chunk();

    Block* getBlock(glm::ivec3 position);
    Block* setBlock(glm::ivec3 position, BlockID block, bool mark = false);

    void draw();
    void updateMesh();
    void markDirty();
    bool isDirty();

    void updateNeighbors(Chunk* x_p = nullptr, Chunk* z_p = nullptr, Chunk* x_m = nullptr, Chunk* z_m = nullptr);
private:
    Block* m_blocks;

    GLuint m_VBO;
    GLuint m_VAO;
    GLuint m_EBO;

    unsigned int m_indices_count;

    bool m_dirty;

    struct {
        Chunk* x_p;
        Chunk* z_p;
        Chunk* x_m;
        Chunk* z_m;
    } m_neighbors;
};