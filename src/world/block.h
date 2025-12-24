#pragma once

#include <string>
#include <unordered_map>
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
	BEDROCK
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
	glm::vec2 uv[4]; // 4 vertices per cube face
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
	SideUV sides[6]; // 6 cube faces
};

struct BlockInfo {
	std::string name = "";
	BlockType block_type;
	BlockModel block_model = BlockModel::CUBE;
	bool is_solid_surface = true;
	bool block_movement = true;
	float layer_height = 0.125f;
	BlockTexture texture = {};
};

inline std::unordered_map<BlockID, BlockInfo> BlocksInfo = {
	{BlockID::EMPTY, {"", BlockType::TRANSPARENT, BlockModel::CUBE, false, false}},
	{BlockID::ERROR, {"error", BlockType::OPAQUE}},
	{BlockID::STONE, {"stone", BlockType::OPAQUE}},
	{BlockID::DIRT, {"dirt", BlockType::OPAQUE}},
	{BlockID::GRASS, {"grass", BlockType::OPAQUE}},
	{BlockID::SNOW_GRASS, {"snow_grass", BlockType::OPAQUE}},
	{BlockID::SAND, {"sand", BlockType::OPAQUE}},
	{BlockID::WATER, {"water", BlockType::TRANSPARENT}},
	{BlockID::BEDROCK, {"bedrock", BlockType::OPAQUE}},
	{BlockID::MY_LOVE, {"love", BlockType::OPAQUE}},
};

class Block {
public:
	Block(BlockID block_id = BlockID::EMPTY, uint8_t metadata = 0);
	~Block();

	BlockID getBlockID() const;
	void setBlockID(BlockID block);

	uint8_t getMetadata() const;
	void setMetadata(uint8_t metadata);

	uint8_t getLayerCount() const;
	void setLayerCount(uint8_t layers);

	float getHeight() const;

private:
	BlockID m_block_id;
	uint8_t m_metadata;
};