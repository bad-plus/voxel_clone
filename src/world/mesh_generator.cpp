#include "mesh_generator.h"
#include "../render/graphics/mesh.h"

ChunkMesh MeshGenerator::generateMesh(
	ChunkStorage* storage,
	ChunkStorage* neighbor_x_plus,
	ChunkStorage* neighbor_z_plus,
	ChunkStorage* neighbor_x_minus,
	ChunkStorage* neighbor_z_minus) {

	m_storage = storage;
	m_neighbor_x_plus = neighbor_x_plus;
	m_neighbor_z_plus = neighbor_z_plus;
	m_neighbor_x_minus = neighbor_x_minus;
	m_neighbor_z_minus = neighbor_z_minus;

	ChunkMesh result;

	if (m_storage == nullptr) return result;

	for (int x = 0; x < ChunkStorage::getSizeX(); x++) {
		for (int y = 0; y < ChunkStorage::getSizeY(); y++) {
			for (int z = 0; z < ChunkStorage::getSizeZ(); z++) {
				const glm::ivec3 block_pos = { x, y, z };
				const BlockID block_id = m_storage->getBlockUnsafe(block_pos)->getBlockID();

				if (block_id == BlockID::EMPTY) continue;

				const BlockTexture& block_texture = BlocksInfo[block_id].texture;
				const BlockType block_type = BlocksInfo[block_id].block_type;

				Mesh* selected_mesh = nullptr;

				if (block_type == BlockType::OPAQUE) {
					selected_mesh = &result.opaque;
				}
				else if (block_type == BlockType::CUTOUT) {
					selected_mesh = &result.cutout;
				}
				else {
					selected_mesh = &result.transparent;
				}

				// TOP Y+
				if (shouldRenderFace(block_id, block_type, { block_pos.x, block_pos.y + 1, block_pos.z })) {
					addBlockFace(selected_mesh, block_pos, BlockSide::TOP, block_texture);
				}

				// BOTTOM Y-
				if (shouldRenderFace(block_id, block_type, { block_pos.x, block_pos.y - 1, block_pos.z })) {
					addBlockFace(selected_mesh, block_pos, BlockSide::BOTTOM, block_texture);
				}

				// FRONT Z+
				if (shouldRenderFace(block_id, block_type, { block_pos.x, block_pos.y, block_pos.z + 1 })) {
					addBlockFace(selected_mesh, block_pos, BlockSide::FRONT, block_texture);
				}

				// BACK Z-
				if (shouldRenderFace(block_id, block_type, { block_pos.x, block_pos.y, block_pos.z - 1 })) {
					addBlockFace(selected_mesh, block_pos, BlockSide::BACK, block_texture);
				}

				// RIGHT X+
				if (shouldRenderFace(block_id, block_type, { block_pos.x + 1, block_pos.y, block_pos.z })) {
					addBlockFace(selected_mesh, block_pos, BlockSide::RIGHT, block_texture);
				}

				// LEFT X-
				if (shouldRenderFace(block_id, block_type, { block_pos.x - 1, block_pos.y, block_pos.z })) {
					addBlockFace(selected_mesh, block_pos, BlockSide::LEFT, block_texture);
				}
			}
		}
	}

	return result;
}

bool MeshGenerator::shouldRenderFace(BlockID current_block_id, BlockType current_block_type, glm::ivec3 neighbor_position) const {
	if (neighbor_position.y < 0 || neighbor_position.y >= ChunkStorage::getSizeY()) {
		return true;
	}

	BlockID neighbor_block = BlockID::EMPTY;

	if (neighbor_position.x < 0) {
		if (m_neighbor_x_minus != nullptr) {
			neighbor_block = m_neighbor_x_minus->getBlockUnsafe({
				ChunkStorage::getSizeX() + neighbor_position.x,
				neighbor_position.y,
				neighbor_position.z
				})->getBlockID();
		}
	}
	else if (neighbor_position.z < 0) {
		if (m_neighbor_z_minus != nullptr) {
			neighbor_block = m_neighbor_z_minus->getBlockUnsafe({
				neighbor_position.x,
				neighbor_position.y,
				ChunkStorage::getSizeZ() + neighbor_position.z
				})->getBlockID();
		}
	}
	else if (neighbor_position.x >= ChunkStorage::getSizeX()) {
		if (m_neighbor_x_plus != nullptr) {
			neighbor_block = m_neighbor_x_plus->getBlockUnsafe({
				neighbor_position.x - ChunkStorage::getSizeX(),
				neighbor_position.y,
				neighbor_position.z
				})->getBlockID();
		}
	}
	else if (neighbor_position.z >= ChunkStorage::getSizeZ()) {
		if (m_neighbor_z_plus != nullptr) {
			neighbor_block = m_neighbor_z_plus->getBlockUnsafe({
				neighbor_position.x,
				neighbor_position.y,
				neighbor_position.z - ChunkStorage::getSizeZ()
				})->getBlockID();
		}
	}
	else {
		neighbor_block = m_storage->getBlockUnsafe(neighbor_position)->getBlockID();
	}

	BlockType neighbor_type = BlocksInfo[neighbor_block].block_type;

	if (current_block_type == BlockType::OPAQUE) {
		return neighbor_type != BlockType::OPAQUE;
	}

	if (current_block_type == BlockType::CUTOUT) {
		return neighbor_type != BlockType::OPAQUE;
	}

	if (current_block_type == BlockType::TRANSPARENT) {
		return current_block_id != neighbor_block;
	}

	return true;
}

bool MeshGenerator::isBlockTransparent(glm::ivec3 block_position) const {
	return shouldRenderFace(BlockID::STONE, BlockType::OPAQUE, block_position);
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