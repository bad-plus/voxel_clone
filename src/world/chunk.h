#pragma once

#include "block.h"

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <mutex>
#include <vector>
#include <atomic>

const unsigned int CHUNK_SIZE_X = 16;
const unsigned int CHUNK_SIZE_Y = 256; // height
const unsigned int CHUNK_SIZE_Z = 16;

struct Chunk;

class Chunk {
public:
    Chunk();
    ~Chunk();

    Block* getBlock(glm::ivec3 position);
    Block* setBlock(glm::ivec3 position, BlockID block, bool mark = false);

    void draw();
    void markDirty();
    bool isDirty();

    void updateNeighbors(Chunk* x_p = nullptr, Chunk* z_p = nullptr, Chunk* x_m = nullptr, Chunk* z_m = nullptr);

    void calculateMesh();
    void uploadMeshToGPU();

    std::vector < BlockID > getBlocksSnapshot();
private:
    Block* getBlockLocal(glm::ivec3 position);

    struct {
        std::vector<GLfloat> vertices;
        std::vector<GLuint> indices;

        std::atomic<bool> ready;
    } m_ready_gpu; // ready to load gpu

    Block* m_blocks;

    GLuint m_VBO;
    GLuint m_VAO;
    GLuint m_EBO;

    unsigned int m_indices_count;

    bool m_dirty;
    std::mutex m_dirty_mutex;

    struct {
        std::atomic<Chunk*> x_p;
        std::atomic<Chunk*> z_p;
        std::atomic<Chunk*> x_m;
        std::atomic<Chunk*> z_m;
    } m_neighbors;
};