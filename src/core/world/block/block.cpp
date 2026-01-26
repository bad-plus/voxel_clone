#include <core/world/block/block.h>

Block::Block(BlockID block_id, uint8_t metadata) {
	m_block_id = block_id;
	m_layers_data = metadata & 0x0F;
}

Block::~Block() {
}

void Block::setBlockID(BlockID block) {
	m_block_id = block;
}

BlockID Block::getBlockID() const {
	return m_block_id;
}

uint8_t Block::getLayerCount() const {
	const BlockInfo& info = GetBlockInfo(m_block_id);

	if (info.block_model != BlockModel::LAYER) {
		return 0;
	}

	return (m_layers_data & 0x07) + 1; // 0-7 → 1-8 layers
}

void Block::setLayerCount(uint8_t layers) {
	const BlockInfo& info = GetBlockInfo(m_block_id);

	if (info.block_model != BlockModel::LAYER) {
		return;
	}

	if (layers < 1) layers = 1;
	if (layers > 8) layers = 8;

	m_layers_data = (layers - 1) & 0x07;
}

float Block::getHeight() const {
	const BlockInfo& info = GetBlockInfo(m_block_id);

	switch (info.block_model) {
	case BlockModel::CUBE:
		return 1.0f;

	case BlockModel::SLAB_BOTTOM:
	case BlockModel::SLAB_TOP:
		return 0.5f;

	case BlockModel::LAYER: {
		uint8_t layers = getLayerCount();
		return layers * info.layer_height;
	}

	case BlockModel::CROSS:
		return 0.0f;

	default:
		return 1.0f;
	}
}