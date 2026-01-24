#include "mesh_generator.h"
#include "../../render/graphics/mesh.h"

#include <glm/glm.hpp>

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

				const BlockInfo& block_info = GetBlockInfo(block_id);
				const BlockTexture& block_texture = block_info.texture;
				const BlockType block_type = block_info.block_type;
				const BlockModel block_model = block_info.block_model;

				Mesh* selected_mesh = nullptr;

				switch (block_type) {
					case BlockType::OPAQUE: {
						selected_mesh = &result.opaque;
						break;
					}
					case BlockType::CUTOUT: {
						selected_mesh = &result.cutout;
						break;
					}
					case BlockType::TRANSPARENT: {
						selected_mesh = &result.transparent;
						break;
					}
				}

				switch (block_model) {
				case BlockModel::CUBE:
					generateCubeModel(selected_mesh, block_id, block_type, block_pos, block_texture);
					break;

				case BlockModel::CROSS:
					generateCrossModel(selected_mesh, block_pos, block_texture);
					break;

				case BlockModel::SLAB_BOTTOM:
					generateSlabModel(selected_mesh, block_id, block_type, block_pos, block_texture, false);
					break;

				case BlockModel::SLAB_TOP:
					generateSlabModel(selected_mesh, block_id, block_type, block_pos, block_texture, true);
					break;

				case BlockModel::LAYER: {
					const Block* block = m_storage->getBlockUnsafe(block_pos);
					uint8_t layer_count = block->getLayerCount();
					if (layer_count == 0) layer_count = 1;
					generateLayerModel(selected_mesh, block_id, block_type, block_pos, block_texture, layer_count);
					break;
				}

				default:
					generateCubeModel(selected_mesh, block_id, block_type, block_pos, block_texture);
					break;
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

	BlockInfo block_info = GetBlockInfo(neighbor_block);

	BlockType neighbor_type = block_info.block_type;
	BlockModel neighbor_model = block_info.block_model;

	if (neighbor_model == BlockModel::CROSS) {
		return true;
	}

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
	if (block_position.y < 0 || block_position.y >= ChunkStorage::getSizeY()) {
		return true;
	} 

	BlockID neighbor_block = BlockID::EMPTY;
	ChunkStorage* target_chunk = nullptr;
	glm::ivec3 local_pos = block_position;

	if (block_position.x < 0) {
		target_chunk = m_neighbor_x_minus;
		local_pos.x = ChunkStorage::getSizeX() + block_position.x;
	}
	else if (block_position.x >= ChunkStorage::getSizeX()) {
		target_chunk = m_neighbor_x_plus;
		local_pos.x = block_position.x - ChunkStorage::getSizeX();
	}
	else if (block_position.z < 0) {
		target_chunk = m_neighbor_z_minus;
		local_pos.z = ChunkStorage::getSizeZ() + block_position.z;
	}
	else if (block_position.z >= ChunkStorage::getSizeZ()) {
		target_chunk = m_neighbor_z_plus;
		local_pos.z = block_position.z - ChunkStorage::getSizeZ();
	}
	else {
		target_chunk = m_storage;
	}

	if (target_chunk == nullptr) {
		return true;
	}

	if (local_pos.x < 0 || local_pos.x >= ChunkStorage::getSizeX() ||
		local_pos.y < 0 || local_pos.y >= ChunkStorage::getSizeY() ||
		local_pos.z < 0 || local_pos.z >= ChunkStorage::getSizeZ()) {
		return true;
	}

	neighbor_block = target_chunk->getBlockUnsafe(local_pos)->getBlockID();

	BlockInfo block_info = GetBlockInfo(neighbor_block);
	BlockType neighbor_type = block_info.block_type;

	return neighbor_type != BlockType::OPAQUE;
}

uint8_t MeshGenerator::calculateVertexAO(
	glm::ivec3 block_pos,
	glm::ivec3 side1,
	glm::ivec3 side2,
	glm::ivec3 corner) const {

	glm::ivec3 pos1 = block_pos + side1;
	glm::ivec3 pos2 = block_pos + side2;
	glm::ivec3 pos_corner = block_pos + corner;

	bool side1_opaque = !isBlockTransparent(pos1);
	bool side2_opaque = !isBlockTransparent(pos2);
	bool corner_opaque = !isBlockTransparent(pos_corner);

	if (side1_opaque && side2_opaque) {
		return 0;
	}

	int opaque_count = (side1_opaque + side2_opaque + corner_opaque);

	return (3 - opaque_count);
}

std::array<uint8_t, 4> MeshGenerator::calculateFaceAO(
	glm::ivec3 block_pos,
	BlockSide side) const {

	using sides_and_corners_t = std::array<glm::ivec3, 3>;
	using aos_param_per_block_side_t = std::array<sides_and_corners_t, 4>;

	static const std::array<aos_param_per_block_side_t, 6> BLOCK_AO_PARAMS = { {
		// TOP
		{{ 
			{ glm::ivec3(-1, 1, 0), glm::ivec3(0, 1, 1), glm::ivec3(-1, 1, 1) },
			{ glm::ivec3(1, 1, 0), glm::ivec3(0, 1, 1), glm::ivec3(1, 1, 1) },
			{ glm::ivec3(1, 1, 0), glm::ivec3(0, 1, -1), glm::ivec3(1, 1, -1) },
			{ glm::ivec3(-1, 1, 0), glm::ivec3(0, 1, -1), glm::ivec3(-1, 1, -1) }
		}},
		// BOTTOM
		{{
			{ glm::ivec3(-1, -1, 0), glm::ivec3(0, -1, -1), glm::ivec3(-1, -1, -1) },
			{ glm::ivec3(1, -1, 0), glm::ivec3(0, -1, -1), glm::ivec3(1, -1, -1) },
			{ glm::ivec3(1, -1, 0), glm::ivec3(0, -1, 1), glm::ivec3(1, -1, 1) },
			{ glm::ivec3(-1, -1, 0), glm::ivec3(0, -1, 1), glm::ivec3(-1, -1, 1) }
		}},
		// LEFT
		{{
			{ glm::ivec3(-1, -1, 0), glm::ivec3(-1, 0, -1), glm::ivec3(-1, -1, -1) },
			{ glm::ivec3(-1, -1, 0), glm::ivec3(-1, 0, 1), glm::ivec3(-1, -1, 1) },
			{ glm::ivec3(-1, 1, 0), glm::ivec3(-1, 0, 1), glm::ivec3(-1, 1, 1) },
			{ glm::ivec3(-1, 1, 0), glm::ivec3(-1, 0, -1), glm::ivec3(-1, 1, -1) }
		}},
		// RIGHT
		{{
			{ glm::ivec3(1, -1, 0), glm::ivec3(1, 0, 1), glm::ivec3(1, -1, 1) },
			{ glm::ivec3(1, -1, 0), glm::ivec3(1, 0, -1), glm::ivec3(1, -1, -1) },
			{ glm::ivec3(1, 1, 0), glm::ivec3(1, 0, -1), glm::ivec3(1, 1, -1) },
			{ glm::ivec3(1, 1, 0), glm::ivec3(1, 0, 1), glm::ivec3(1, 1, 1) }
		}},
		// FRONT
		{{
			{ glm::ivec3(-1, 0, 1), glm::ivec3(0, -1, 1), glm::ivec3(-1, -1, 1) },
			{ glm::ivec3(1, 0, 1), glm::ivec3(0, -1, 1), glm::ivec3(1, -1, 1) },
			{ glm::ivec3(1, 0, 1), glm::ivec3(0, 1, 1), glm::ivec3(1, 1, 1) },
			{ glm::ivec3(-1, 0, 1), glm::ivec3(0, 1, 1), glm::ivec3(-1, 1, 1) }
		}},
		// BACK
		{{
			{ glm::ivec3(1, 0, -1), glm::ivec3(0, -1, -1), glm::ivec3(1, -1, -1) },
			{ glm::ivec3(-1, 0, -1), glm::ivec3(0, -1, -1), glm::ivec3(-1, -1, -1) },
			{ glm::ivec3(-1, 0, -1), glm::ivec3(0, 1, -1), glm::ivec3(-1, 1, -1) },
			{ glm::ivec3(1, 0, -1), glm::ivec3(0, 1, -1), glm::ivec3(1, 1, -1) }
		}}
	} };

	const aos_param_per_block_side_t& block_params = BLOCK_AO_PARAMS[std::to_underlying(side)];

	std::array<uint8_t, 4> aos;
	for (int i = 0; i < 4; i++) {
		aos[i] = calculateVertexAO(
			block_pos,
			block_params[i][0],
			block_params[i][1],
			block_params[i][2]
		);
	}

	return aos;
}


void MeshGenerator::addBlockFace( 
	Mesh* mesh,
	glm::vec3 block_position,
	BlockSide block_side,
	const BlockTexture& texture)
{
	const SideUV face_uv = texture.sides[std::to_underlying(block_side)];
	const GLuint vertex_count = static_cast<GLuint>(mesh->getVertexCount());

	const int side_index = std::to_underlying(block_side);
	const auto vertices = CUBE_VERTICES[side_index];
	const glm::vec3& normal = FACE_NORMALS[block_side];

	std::array<uint8_t, 4> aos = calculateFaceAO(glm::ivec3(block_position), block_side);

	for (int i = 0; i < 4; i++) {
		glm::vec3 position = {
			block_position.x + vertices[i * 3 + 0],
			block_position.y + vertices[i * 3 + 1],
			block_position.z + vertices[i * 3 + 2]
		};

		Light light;
		light.set(0, 0, 0, 15);

		mesh->addVertex( 
			position,
			normal,
			{ face_uv.uv[i].x, face_uv.uv[i].y },
			light,
			aos[i] 
		);
	}

	bool flip = (aos[0] + aos[2]) > (aos[1] + aos[3]);

	if (flip) {
		mesh->addTriangle(vertex_count + 0, vertex_count + 1, vertex_count + 3);
		mesh->addTriangle(vertex_count + 1, vertex_count + 2, vertex_count + 3);
	}
	else {
		mesh->addTriangle(vertex_count + 0, vertex_count + 1, vertex_count + 2);
		mesh->addTriangle(vertex_count + 2, vertex_count + 3, vertex_count + 0);
	}
}