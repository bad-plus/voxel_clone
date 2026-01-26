#pragma once
#include "../../render/graphics/mesh.h"
#include <core/world/chunk/chunk_storage.h>
#include <unordered_map>

struct ChunkMesh {
	Mesh opaque;
	Mesh cutout;
	Mesh transparent;
};

class MeshGenerator {
public:
	MeshGenerator() = default;
	~MeshGenerator() = default;

	ChunkMesh generateMesh(
		ChunkStorage* storage,
		ChunkStorage* neighbor_x_plus = nullptr,
		ChunkStorage* neighbor_z_plus = nullptr,
		ChunkStorage* neighbor_x_minus = nullptr,
		ChunkStorage* neighbor_z_minus = nullptr
	);

private:
	ChunkStorage* m_storage = nullptr;
	ChunkStorage* m_neighbor_x_plus = nullptr;
	ChunkStorage* m_neighbor_z_plus = nullptr;
	ChunkStorage* m_neighbor_x_minus = nullptr;
	ChunkStorage* m_neighbor_z_minus = nullptr;

	void generateCubeModel(
		Mesh* mesh,
		BlockID block_id,
		BlockType block_type,
		glm::ivec3 block_pos,
		const BlockTexture& texture
	);

	void generateCrossModel(
		Mesh* mesh,
		glm::ivec3 block_pos,
		const BlockTexture& texture
	);

	void generateSlabModel(
		Mesh* mesh,
		BlockID block_id,
		BlockType block_type,
		glm::ivec3 block_pos,
		const BlockTexture& texture,
		bool is_top
	);

	void generateLayerModel(
		Mesh* mesh,
		BlockID block_id,
		BlockType block_type,
		glm::ivec3 block_pos,
		const BlockTexture& texture,
		uint8_t layer_count
	);

	bool shouldRenderFace(
		BlockID current_block_id,
		BlockType current_block_type,
		glm::ivec3 neighbor_position
	) const;

	bool isBlockTransparent(
		glm::ivec3 block_position
	) const;

	uint8_t calculateVertexAO(
		glm::ivec3 block_pos,
		glm::ivec3 side1,
		glm::ivec3 side2,
		glm::ivec3 corner
	) const;

	std::array<uint8_t, 4> calculateFaceAO(
		glm::ivec3 block_pos,
		BlockSide side
	) const;

	void addBlockFace(
		Mesh* mesh,
		glm::vec3 block_position,
		BlockSide block_side,
		const BlockTexture& texture
	);

	inline static std::unordered_map<BlockSide, glm::vec3> FACE_NORMALS = {
		{ BlockSide::TOP, {0.0f, 1.0f, 0.0f} },
		{ BlockSide::BOTTOM, {0.0f, -1.0f, 0.0f} },
		{ BlockSide::LEFT, {-1.0f, 0.0f, 0.0f} },
		{ BlockSide::RIGHT, {1.0f, 0.0f, 0.0f} },
		{ BlockSide::FRONT, {0.0f, 0.0f, 1.0f} },
		{ BlockSide::BACK, {0.0f, 0.0f, -1.0f} }
	};

	using FACE_VERTICES_TYPE = std::array<float, 12>;
	using CUBE_VERTICES_TYPE = std::array<FACE_VERTICES_TYPE, 6>;

	CUBE_VERTICES_TYPE CUBE_VERTICES = { {
			// TOP (Y+)
			{ 0.0f, 1.0f, 1.0f,  1.0f, 1.0f, 1.0f,  1.0f, 1.0f, 0.0f,  0.0f, 1.0f, 0.0f },
			// BOTTOM (Y-)
			{ 0.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f, 1.0f,  0.0f, 0.0f, 1.0f },
			// LEFT (X-)
			{ 0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f, 1.0f,  0.0f, 1.0f, 0.0f },
			// RIGHT (X+)
			{ 1.0f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f, 0.0f,  1.0f, 1.0f, 1.0f },
			// FRONT (Z+)
			{ 0.0f, 0.0f, 1.0f,  1.0f, 0.0f, 1.0f,  1.0f, 1.0f, 1.0f,  0.0f, 1.0f, 1.0f },
			// BACK (Z-)
			{ 1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f, 0.0f }
		} };
};