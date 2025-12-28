#pragma once

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

		data[std::to_underlying(BlockID::EMPTY)] = { 
			.name = "", 
			.block_type = BlockType::TRANSPARENT, 
			.block_model = BlockModel::CUBE, 
			.is_solid_surface = false, 
			.block_movement = false 
		};
		data[std::to_underlying(BlockID::ERROR)] = { 
			.name = "error"
		};
		data[std::to_underlying(BlockID::STONE)] = { 
			.name = "stone"
		};
		data[std::to_underlying(BlockID::DIRT)] = { 
			.name = "dirt"
		};
		data[std::to_underlying(BlockID::GRASS)] = { 
			.name = "grass"
		};
		data[std::to_underlying(BlockID::SNOW_GRASS)] = { 
			.name = "snow_grass"
		};
		data[std::to_underlying(BlockID::SAND)] = { 
			.name = "sand"
		};
		data[std::to_underlying(BlockID::WATER)] = { 
			.name = "water"
		};
		data[std::to_underlying(BlockID::BEDROCK)] = { 
			.name = "bedrock"
		};
		data[std::to_underlying(BlockID::MY_LOVE)] = { 
			.name = "love"
		};
		data[std::to_underlying(BlockID::TREE_LEAVES)] = { 
			.name = "tree_leaves"
		};
		data[std::to_underlying(BlockID::RED)] = {
			.name = "red",
		};
		data[std::to_underlying(BlockID::GREEN)] = { 
			.name = "green",
		};
		data[std::to_underlying(BlockID::BLUE)] = { 
			.name = "blue",
		};
		data[std::to_underlying(BlockID::GRAY)] = { 
			.name = "gray",
		};

		return data;
	}()
};

inline BlockInfo& GetBlockInfo(BlockID id) {
	return BlocksInfo[std::to_underlying(id)];
}