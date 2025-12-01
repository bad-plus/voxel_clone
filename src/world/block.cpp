#include "block.h"

Block::Block(BlockID block_id) {
    m_block_id = block_id;
}
Block::~Block() {

}

void Block::setBlockID(BlockID block) {
    m_block_id = block;
}

BlockID Block::getBlockID() {
    return m_block_id;
}