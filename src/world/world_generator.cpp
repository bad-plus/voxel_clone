#include "world_generator.h"
#include "block.h"
#include "chunk.h"

WorldGeneator::WorldGeneator(int seed) {
    m_seed = seed;
}
WorldGeneator::~WorldGeneator() {

}

Chunk* WorldGeneator::generateChunk(int x, int z) {
    Chunk* chunk = new Chunk();

    for(int px = 0; px < CHUNK_SIZE_X; px++) {
        for(int pz = 0; pz < CHUNK_SIZE_Z; pz++) {
            for(int py = 0; py < CHUNK_SIZE_Y; py++) {
                if(py < 10) chunk->setBlock({px, py, pz}, BlockID::STONE, false);
            }
        }
    }
    return chunk;
}