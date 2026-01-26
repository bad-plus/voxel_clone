#pragma once
#include <array>
#include <glm/glm.hpp>
//#include "../lighting/light.h"

using light_t = uint16_t; // TODO: TEMP

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

enum class BlockSide {
	TOP = 0,
	BOTTOM = 1,
	LEFT = 2,
	RIGHT = 3,
	FRONT = 4,
	BACK = 5
};

struct SideUV {
	std::array<glm::vec2, 4> uv; // 4 vertices per cube face
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
	bool lamp = false;
	light_t lamp_color = 0;
	BlockTexture texture = {};
};