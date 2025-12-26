#pragma once

enum class BlockID : uint8_t {
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
	TREE_LEAVES,
	BLUE,
	RED,
	GREEN,
	GRAY,
	COUNT
};

inline std::array<BlockInfo, std::to_underlying(BlockID::COUNT)> BlocksInfo = {
	[] {
		std::array<BlockInfo, std::to_underlying(BlockID::COUNT)> data;

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
		data[std::to_underlying(BlockID::TREE_LEAVES)] = { "tree_leaves", BlockType::CUTOUT };
		data[std::to_underlying(BlockID::RED)] = { "red", BlockType::OPAQUE };
		data[std::to_underlying(BlockID::GREEN)] = { "green", BlockType::OPAQUE };
		data[std::to_underlying(BlockID::BLUE)] = { "blue", BlockType::OPAQUE };
		data[std::to_underlying(BlockID::GRAY)] = { "gray", BlockType::OPAQUE };

		return data;
	}()
};

inline BlockInfo& GetBlockInfo(BlockID id) {
	return BlocksInfo[std::to_underlying(id)];
}