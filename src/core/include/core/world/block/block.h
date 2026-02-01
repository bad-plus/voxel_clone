#pragma once

#include <string>
#include <array>
#include <glm/glm.hpp>
#include <cstdint>

#include "block_data.h"
#include "block_list.h"

class Block {
public:
	Block(BlockID block_id = BlockID::EMPTY, uint8_t layers = 0);
	~Block();

	BlockID getBlockID() const;
	void setBlockID(BlockID block);

	uint8_t getLayerCount() const;
	void setLayerCount(uint8_t layers);

	float getHeight() const;

	std::vector<uint8_t> to_bytes();
	void from_bytes(const std::vector<uint8_t>& bytes);
private:
	BlockID m_block_id;
	uint8_t m_layers_data;
};