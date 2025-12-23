#pragma once
#include "../render/graphics/mesh.h"
#include "chunk_storage.h"
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

	bool shouldRenderFace(
		BlockID current_block_id,
		BlockType current_block_type,
		glm::ivec3 neighbor_position
	) const;

	bool isBlockTransparent(
		glm::ivec3 block_position
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

	inline static const float FACE_VERTICES[6][12] = {
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
	};
};