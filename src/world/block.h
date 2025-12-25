#pragma once

#include <string>
#include <array>
#include <glm/glm.hpp>
#include <cstdint>


enum class BlockID {
	EMPTY = 0,
	ERROR,
	STONE,
	DIRT,
	GRASS,
	SNOW_GRASS,
	MY_LOVE,
	SAND,
	WATER,
	BEDROCK,
	COUNT
};

enum class BlockType {
	OPAQUE = 0,
	CUTOUT,
	TRANSPARENT
};

enum class BlockModel {
	CUBE = 0,
	CROSS,
	SLAB_BOTTOM,
	SLAB_TOP,
	LAYER,
	CUSTOM
};

struct SideUV {
	std::array<glm::vec2, 4> uv; // 4 vertices per cube face
};

enum class BlockSide {
	TOP = 0,
	BOTTOM = 1,
	LEFT = 2,
	RIGHT = 3,
	FRONT = 4,
	BACK = 5
};

struct BlockTexture {
	std::array<SideUV, 6> sides; // 6 cube faces
};

struct BlockInfo {
	std::string name = "";
	BlockType block_type = BlockType::OPAQUE;
	BlockModel block_model = BlockModel::CUBE;
	bool is_solid_surface = true;
	bool block_movement = true;
	float layer_height = 0.125f;
	BlockTexture texture = {};
};

inline std::array<BlockInfo, (size_t)BlockID::COUNT> BlocksInfo = {
	[] {
		std::array<BlockInfo, (size_t)BlockID::COUNT> data;

		data[std::to_underlying(BlockID::EMPTY)] = { "", BlockType::TRANSPARENT, BlockModel::CUBE, false, false };
		data[std::to_underlying(BlockID::ERROR)] = { "error", BlockType::OPAQUE };
		data[std::to_underlying(BlockID::STONE)] = { "stone", BlockType::OPAQUE };
		data[std::to_underlying(BlockID::DIRT)] = { "dirt", BlockType::OPAQUE };
		data[std::to_underlying(BlockID::GRASS)] = { "grass", BlockType::OPAQUE };
		data[std::to_underlying(BlockID::SNOW_GRASS)] = { "snow_grass", BlockType::OPAQUE };
		data[std::to_underlying(BlockID::SAND)] = { "sand", BlockType::OPAQUE };
		data[std::to_underlying(BlockID::WATER)] = { "water", BlockType::TRANSPARENT };
		data[std::to_underlying(BlockID::BEDROCK)] = { "bedrock", BlockType::OPAQUE };
		data[std::to_underlying(BlockID::MY_LOVE)] = { "love", BlockType::OPAQUE };

		return data;
	}()
};

inline BlockInfo& GetBlockInfo(BlockID id) {
	return BlocksInfo[std::to_underlying(id)];
}

class Block {
public:
	Block(BlockID block_id = BlockID::EMPTY, uint8_t layers = 0);
	~Block();

	BlockID getBlockID() const;
	void setBlockID(BlockID block);

	uint8_t getLayerCount() const;
	void setLayerCount(uint8_t layers);

	float getHeight() const;

private:
	BlockID m_block_id;
	uint8_t m_layers_data;
};