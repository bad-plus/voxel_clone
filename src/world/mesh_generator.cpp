#include "mesh_generator.h"
#include "../render/graphics/mesh.h"

Mesh MeshGenerator::generateMesh(
	ChunkStorage* storage,
	ChunkStorage* neighbor_x_plus,
	ChunkStorage* neighbor_z_plus,
	ChunkStorage* neighbor_x_minus,
	ChunkStorage* neighbor_z_minus)
{
	if (!storage) return Mesh();

	m_storage = storage;
	m_neighbor_x_plus = neighbor_x_plus;
	m_neighbor_z_plus = neighbor_z_plus;
	m_neighbor_x_minus = neighbor_x_minus;
	m_neighbor_z_minus = neighbor_z_minus;

	Mesh result_mesh;

	const size_t estimated_vertices = ChunkStorage::getSizeX() *
		ChunkStorage::getSizeY() *
		ChunkStorage::getSizeZ() * 6 * 4;


	for (int x = 0; x < ChunkStorage::getSizeX(); x++) {
		for (int y = 0; y < ChunkStorage::getSizeY(); y++) {
			for (int z = 0; z < ChunkStorage::getSizeZ(); z++) {
				const glm::ivec3 block_pos = { x, y, z };
				const BlockID block_id = m_storage->getBlockUnsafe(block_pos)->getBlockID();

				if (block_id == BlockID::EMPTY) continue;

				const BlockTexture& block_texture = BlocksInfo[block_id].texture;

				// TOP Y+
				if (isBlockTransparent({ block_pos.x, block_pos.y + 1, block_pos.z })) {
					addBlockFace(&result_mesh, block_pos, BlockSide::TOP, block_texture);
				}

				// BOTTOM Y-
				if (isBlockTransparent({ block_pos.x, block_pos.y - 1, block_pos.z })) {
					addBlockFace(&result_mesh, block_pos, BlockSide::BOTTOM, block_texture);
				}

				// FRONT Z+
				if (isBlockTransparent({ block_pos.x, block_pos.y, block_pos.z + 1 })) {
					addBlockFace(&result_mesh, block_pos, BlockSide::FRONT, block_texture);
				}

				// BACK Z-
				if (isBlockTransparent({ block_pos.x, block_pos.y, block_pos.z - 1 })) {
					addBlockFace(&result_mesh, block_pos, BlockSide::BACK, block_texture);
				}

				// RIGHT X+
				if (isBlockTransparent({ block_pos.x + 1, block_pos.y, block_pos.z })) {
					addBlockFace(&result_mesh, block_pos, BlockSide::RIGHT, block_texture);
				}

				// LEFT X-
				if (isBlockTransparent({ block_pos.x - 1, block_pos.y, block_pos.z })) {
					addBlockFace(&result_mesh, block_pos, BlockSide::LEFT, block_texture);
				}
			}
		}
	}

	return result_mesh;
}

bool MeshGenerator::isBlockTransparent(
	glm::ivec3 block_position
) const {
	BlockID selected_block = BlockID::EMPTY;
	if (block_position.y < 0 || block_position.y >= ChunkStorage::getSizeY()) return true;

	if (block_position.x < 0) {
		if (m_neighbor_x_minus != nullptr) {
			selected_block = m_neighbor_x_minus->getBlockUnsafe({ ChunkStorage::getSizeX() + block_position.x, block_position.y, block_position.z })->getBlockID();
		}
	}
	else if (block_position.z < 0) {
		if (m_neighbor_z_minus != nullptr) {
			selected_block = m_neighbor_z_minus->getBlockUnsafe({ block_position.x, block_position.y, ChunkStorage::getSizeZ() + block_position.z })->getBlockID();
		}
	}
	else if (block_position.x >= ChunkStorage::getSizeX()) {
		if (m_neighbor_x_plus != nullptr) {
			selected_block = m_neighbor_x_plus->getBlockUnsafe({ block_position.x - ChunkStorage::getSizeX(), block_position.y, block_position.z })->getBlockID();
		}
	}
	else if (block_position.z >= ChunkStorage::getSizeZ()) {
		if (m_neighbor_z_plus != nullptr) {
			selected_block = m_neighbor_z_plus->getBlockUnsafe({ block_position.x, block_position.y, block_position.z - ChunkStorage::getSizeZ() })->getBlockID();
		}
	}
	else selected_block = m_storage->getBlockUnsafe(block_position)->getBlockID();

	return BlocksInfo[selected_block].isTransparent;
}

void MeshGenerator::addBlockFace(
	Mesh* mesh,
	glm::vec3 block_position,
	BlockSide block_side,
	const BlockTexture& texture)
{
	SideUV face_uv = texture.sides[(unsigned int)block_side];
	GLuint vertex_count = mesh->getVertexCount();

	const int side_index = (int)block_side;
	const float* vertices = FACE_VERTICES[side_index];
	const glm::vec3& normal = FACE_NORMALS[block_side];

	for (int i = 0; i < 4; i++) {
		glm::vec3 position = {
			block_position.x + vertices[i * 3 + 0],
			block_position.y + vertices[i * 3 + 1],
			block_position.z + vertices[i * 3 + 2]
		};

		mesh->addVertex(
			position,
			normal,
			{ face_uv.uv[i].x, face_uv.uv[i].y }
		);
	}

	mesh->addTriangle(vertex_count + 0, vertex_count + 1, vertex_count + 2);
	mesh->addTriangle(vertex_count + 2, vertex_count + 3, vertex_count + 0);
}