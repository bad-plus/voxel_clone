#include "../mesh_generator.h"

void MeshGenerator::generateCubeModel(
	Mesh* mesh,
	BlockID block_id,
	BlockType block_type,
	glm::ivec3 block_pos,
	const BlockTexture& texture) {

	constexpr std::array < std::pair < BlockSide, glm::ivec3>, 6> offsets = { {
		{BlockSide::TOP, {0, 1, 0}},
		{BlockSide::BOTTOM, {0, -1, 0}},
		{BlockSide::FRONT, {0, 0, 1}},
		{BlockSide::BACK, {0, 0, -1}},
		{BlockSide::RIGHT, {1, 0, 0}},
		{BlockSide::LEFT, {-1, 0, 0}}
	} };

	for (const auto& [key, value] : offsets) {
		if (shouldRenderFace(block_id, block_type, block_pos + value)) {
			addBlockFace(mesh, block_pos, key, texture);
		}
	}
}
