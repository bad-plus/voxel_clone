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

				const BlockInfo& block_info = BlocksInfo[block_id];
				const BlockTexture& block_texture = block_info.texture;
				const BlockType block_type = block_info.block_type;
				const BlockModel block_model = block_info.block_model;

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
					Block* block = m_storage->getBlockUnsafe(block_pos);
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

void MeshGenerator::generateCubeModel(
	Mesh* mesh,
	BlockID block_id,
	BlockType block_type,
	glm::ivec3 block_pos,
	const BlockTexture& texture) {

	if (shouldRenderFace(block_id, block_type, { block_pos.x, block_pos.y + 1, block_pos.z })) {
		addBlockFace(mesh, block_pos, BlockSide::TOP, texture);
	}

	if (shouldRenderFace(block_id, block_type, { block_pos.x, block_pos.y - 1, block_pos.z })) {
		addBlockFace(mesh, block_pos, BlockSide::BOTTOM, texture);
	}

	if (shouldRenderFace(block_id, block_type, { block_pos.x, block_pos.y, block_pos.z + 1 })) {
		addBlockFace(mesh, block_pos, BlockSide::FRONT, texture);
	}

	if (shouldRenderFace(block_id, block_type, { block_pos.x, block_pos.y, block_pos.z - 1 })) {
		addBlockFace(mesh, block_pos, BlockSide::BACK, texture);
	}

	if (shouldRenderFace(block_id, block_type, { block_pos.x + 1, block_pos.y, block_pos.z })) {
		addBlockFace(mesh, block_pos, BlockSide::RIGHT, texture);
	}

	if (shouldRenderFace(block_id, block_type, { block_pos.x - 1, block_pos.y, block_pos.z })) {
		addBlockFace(mesh, block_pos, BlockSide::LEFT, texture);
	}
}

void MeshGenerator::generateCrossModel(
	Mesh* mesh,
	glm::ivec3 block_pos,
	const BlockTexture& texture) {

	SideUV face_uv = texture.sides[0];
	GLuint vertex_count = mesh->getVertexCount();

	glm::vec3 base_pos = glm::vec3(block_pos);

	glm::vec3 quad1_verts[4] = {
		base_pos + glm::vec3(0.0f, 0.0f, 0.0f),
		base_pos + glm::vec3(1.0f, 0.0f, 1.0f),
		base_pos + glm::vec3(1.0f, 1.0f, 1.0f),
		base_pos + glm::vec3(0.0f, 1.0f, 0.0f)
	};

	glm::vec3 quad2_verts[4] = {
		base_pos + glm::vec3(1.0f, 0.0f, 0.0f),
		base_pos + glm::vec3(0.0f, 0.0f, 1.0f),
		base_pos + glm::vec3(0.0f, 1.0f, 1.0f),
		base_pos + glm::vec3(1.0f, 1.0f, 0.0f)
	};

	glm::vec3 normal1 = glm::normalize(glm::cross(
		quad1_verts[1] - quad1_verts[0],
		quad1_verts[2] - quad1_verts[0]
	));

	glm::vec3 normal2 = glm::normalize(glm::cross(
		quad2_verts[1] - quad2_verts[0],
		quad2_verts[2] - quad2_verts[0]
	));

	for (int i = 0; i < 4; i++) {
		mesh->addVertex(quad1_verts[i], normal1, { face_uv.uv[i].x, face_uv.uv[i].y });
	}
	mesh->addTriangle(vertex_count + 0, vertex_count + 1, vertex_count + 2);
	mesh->addTriangle(vertex_count + 2, vertex_count + 3, vertex_count + 0);
	mesh->addTriangle(vertex_count + 2, vertex_count + 1, vertex_count + 0);
	mesh->addTriangle(vertex_count + 0, vertex_count + 3, vertex_count + 2);

	vertex_count = mesh->getVertexCount();

	for (int i = 0; i < 4; i++) {
		mesh->addVertex(quad2_verts[i], normal2, { face_uv.uv[i].x, face_uv.uv[i].y });
	}
	mesh->addTriangle(vertex_count + 0, vertex_count + 1, vertex_count + 2);
	mesh->addTriangle(vertex_count + 2, vertex_count + 3, vertex_count + 0);
	mesh->addTriangle(vertex_count + 2, vertex_count + 1, vertex_count + 0);
	mesh->addTriangle(vertex_count + 0, vertex_count + 3, vertex_count + 2);
}

void MeshGenerator::generateSlabModel(
	Mesh* mesh,
	BlockID block_id,
	BlockType block_type,
	glm::ivec3 block_pos,
	const BlockTexture& texture,
	bool is_top) {

	float y_offset = is_top ? 0.5f : 0.0f;
	float height = 0.5f;

	SideUV top_uv = texture.sides[(int)BlockSide::TOP];
	SideUV bottom_uv = texture.sides[(int)BlockSide::BOTTOM];
	SideUV side_uv = texture.sides[(int)BlockSide::FRONT];

	GLuint vertex_count = mesh->getVertexCount();
	glm::vec3 base_pos = glm::vec3(block_pos);

	// TOP
	if (is_top || shouldRenderFace(block_id, block_type, { block_pos.x, block_pos.y + 1, block_pos.z })) {
		glm::vec3 verts[4] = {
			base_pos + glm::vec3(0.0f, y_offset + height, 1.0f),
			base_pos + glm::vec3(1.0f, y_offset + height, 1.0f),
			base_pos + glm::vec3(1.0f, y_offset + height, 0.0f),
			base_pos + glm::vec3(0.0f, y_offset + height, 0.0f)
		};
		for (int i = 0; i < 4; i++) {
			mesh->addVertex(verts[i], { 0, 1, 0 }, { top_uv.uv[i].x, top_uv.uv[i].y });
		}
		mesh->addTriangle(vertex_count + 0, vertex_count + 1, vertex_count + 2);
		mesh->addTriangle(vertex_count + 2, vertex_count + 3, vertex_count + 0);
		vertex_count += 4;
	}

	// BOTTOM
	if (!is_top || shouldRenderFace(block_id, block_type, { block_pos.x, block_pos.y - 1, block_pos.z })) {
		glm::vec3 verts[4] = {
			base_pos + glm::vec3(0.0f, y_offset, 0.0f),
			base_pos + glm::vec3(1.0f, y_offset, 0.0f),
			base_pos + glm::vec3(1.0f, y_offset, 1.0f),
			base_pos + glm::vec3(0.0f, y_offset, 1.0f)
		};
		for (int i = 0; i < 4; i++) {
			mesh->addVertex(verts[i], { 0, -1, 0 }, { bottom_uv.uv[i].x, bottom_uv.uv[i].y });
		}
		mesh->addTriangle(vertex_count + 0, vertex_count + 1, vertex_count + 2);
		mesh->addTriangle(vertex_count + 2, vertex_count + 3, vertex_count + 0);
		vertex_count += 4;
	}

	
	if (shouldRenderFace(block_id, block_type, { block_pos.x, block_pos.y, block_pos.z + 1 })) {
		glm::vec3 verts[4] = {
			base_pos + glm::vec3(0.0f, y_offset, 1.0f),
			base_pos + glm::vec3(1.0f, y_offset, 1.0f),
			base_pos + glm::vec3(1.0f, y_offset + height, 1.0f),
			base_pos + glm::vec3(0.0f, y_offset + height, 1.0f)
		};
		glm::vec2 uv_coords[4] = {
			{side_uv.uv[0].x, side_uv.uv[0].y},
			{side_uv.uv[1].x, side_uv.uv[1].y},
			{side_uv.uv[2].x, side_uv.uv[0].y + (side_uv.uv[2].y - side_uv.uv[0].y) * 0.5f},
			{side_uv.uv[3].x, side_uv.uv[0].y + (side_uv.uv[3].y - side_uv.uv[0].y) * 0.5f}
		};
		for (int i = 0; i < 4; i++) {
			mesh->addVertex(verts[i], { 0, 0, 1 }, uv_coords[i]);
		}
		mesh->addTriangle(vertex_count + 0, vertex_count + 1, vertex_count + 2);
		mesh->addTriangle(vertex_count + 2, vertex_count + 3, vertex_count + 0);
	}
}

void MeshGenerator::generateLayerModel(
	Mesh* mesh,
	BlockID block_id,
	BlockType block_type,
	glm::ivec3 block_pos,
	const BlockTexture& texture,
	uint8_t layer_count) {

	const BlockInfo& info = BlocksInfo[block_id];
	float height = layer_count * info.layer_height;

	if (height >= 1.0f) {
		generateCubeModel(mesh, block_id, block_type, block_pos, texture);
		return;
	}

	SideUV top_uv = texture.sides[(int)BlockSide::TOP];
	SideUV bottom_uv = texture.sides[(int)BlockSide::BOTTOM];
	SideUV side_uv = texture.sides[(int)BlockSide::FRONT];

	GLuint vertex_count = mesh->getVertexCount();
	glm::vec3 base_pos = glm::vec3(block_pos);

	// TOP
	{
		glm::vec3 verts[4] = {
			base_pos + glm::vec3(0.0f, height, 1.0f),
			base_pos + glm::vec3(1.0f, height, 1.0f),
			base_pos + glm::vec3(1.0f, height, 0.0f),
			base_pos + glm::vec3(0.0f, height, 0.0f)
		};
		for (int i = 0; i < 4; i++) {
			mesh->addVertex(verts[i], { 0, 1, 0 }, { top_uv.uv[i].x, top_uv.uv[i].y });
		}
		mesh->addTriangle(vertex_count + 0, vertex_count + 1, vertex_count + 2);
		mesh->addTriangle(vertex_count + 2, vertex_count + 3, vertex_count + 0);
		vertex_count += 4;
	}

	// BOTTOM
	if (shouldRenderFace(block_id, block_type, { block_pos.x, block_pos.y - 1, block_pos.z })) {
		glm::vec3 verts[4] = {
			base_pos + glm::vec3(0.0f, 0.0f, 0.0f),
			base_pos + glm::vec3(1.0f, 0.0f, 0.0f),
			base_pos + glm::vec3(1.0f, 0.0f, 1.0f),
			base_pos + glm::vec3(0.0f, 0.0f, 1.0f)
		};
		for (int i = 0; i < 4; i++) {
			mesh->addVertex(verts[i], { 0, -1, 0 }, { bottom_uv.uv[i].x, bottom_uv.uv[i].y });
		}
		mesh->addTriangle(vertex_count + 0, vertex_count + 1, vertex_count + 2);
		mesh->addTriangle(vertex_count + 2, vertex_count + 3, vertex_count + 0);
		vertex_count += 4;
	}

	if (shouldRenderFace(block_id, block_type, { block_pos.x, block_pos.y, block_pos.z + 1 })) {
		glm::vec3 verts[4] = {
			base_pos + glm::vec3(0.0f, 0.0f, 1.0f),
			base_pos + glm::vec3(1.0f, 0.0f, 1.0f),
			base_pos + glm::vec3(1.0f, height, 1.0f),
			base_pos + glm::vec3(0.0f, height, 1.0f)
		};
		glm::vec2 uv_coords[4] = {
			{side_uv.uv[0].x, side_uv.uv[0].y},
			{side_uv.uv[1].x, side_uv.uv[1].y},
			{side_uv.uv[2].x, side_uv.uv[0].y + (side_uv.uv[2].y - side_uv.uv[0].y) * height},
			{side_uv.uv[3].x, side_uv.uv[0].y + (side_uv.uv[3].y - side_uv.uv[0].y) * height}
		};
		for (int i = 0; i < 4; i++) {
			mesh->addVertex(verts[i], { 0, 0, 1 }, uv_coords[i]);
		}
		mesh->addTriangle(vertex_count + 0, vertex_count + 1, vertex_count + 2);
		mesh->addTriangle(vertex_count + 2, vertex_count + 3, vertex_count + 0);
		vertex_count += 4;
	}

	if (shouldRenderFace(block_id, block_type, { block_pos.x, block_pos.y, block_pos.z - 1 })) {
		glm::vec3 verts[4] = {
			base_pos + glm::vec3(1.0f, 0.0f, 0.0f),
			base_pos + glm::vec3(0.0f, 0.0f, 0.0f),
			base_pos + glm::vec3(0.0f, height, 0.0f),
			base_pos + glm::vec3(1.0f, height, 0.0f)
		};
		glm::vec2 uv_coords[4] = {
			{side_uv.uv[0].x, side_uv.uv[0].y},
			{side_uv.uv[1].x, side_uv.uv[1].y},
			{side_uv.uv[2].x, side_uv.uv[0].y + (side_uv.uv[2].y - side_uv.uv[0].y) * height},
			{side_uv.uv[3].x, side_uv.uv[0].y + (side_uv.uv[3].y - side_uv.uv[0].y) * height}
		};
		for (int i = 0; i < 4; i++) {
			mesh->addVertex(verts[i], { 0, 0, -1 }, uv_coords[i]);
		}
		mesh->addTriangle(vertex_count + 0, vertex_count + 1, vertex_count + 2);
		mesh->addTriangle(vertex_count + 2, vertex_count + 3, vertex_count + 0);
		vertex_count += 4;
	}

	if (shouldRenderFace(block_id, block_type, { block_pos.x + 1, block_pos.y, block_pos.z })) {
		glm::vec3 verts[4] = {
			base_pos + glm::vec3(1.0f, 0.0f, 1.0f),
			base_pos + glm::vec3(1.0f, 0.0f, 0.0f),
			base_pos + glm::vec3(1.0f, height, 0.0f),
			base_pos + glm::vec3(1.0f, height, 1.0f)
		};
		glm::vec2 uv_coords[4] = {
			{side_uv.uv[0].x, side_uv.uv[0].y},
			{side_uv.uv[1].x, side_uv.uv[1].y},
			{side_uv.uv[2].x, side_uv.uv[0].y + (side_uv.uv[2].y - side_uv.uv[0].y) * height},
			{side_uv.uv[3].x, side_uv.uv[0].y + (side_uv.uv[3].y - side_uv.uv[0].y) * height}
		};
		for (int i = 0; i < 4; i++) {
			mesh->addVertex(verts[i], { 1, 0, 0 }, uv_coords[i]);
		}
		mesh->addTriangle(vertex_count + 0, vertex_count + 1, vertex_count + 2);
		mesh->addTriangle(vertex_count + 2, vertex_count + 3, vertex_count + 0);
		vertex_count += 4;
	}

	if (shouldRenderFace(block_id, block_type, { block_pos.x - 1, block_pos.y, block_pos.z })) {
		glm::vec3 verts[4] = {
			base_pos + glm::vec3(0.0f, 0.0f, 0.0f),
			base_pos + glm::vec3(0.0f, 0.0f, 1.0f),
			base_pos + glm::vec3(0.0f, height, 1.0f),
			base_pos + glm::vec3(0.0f, height, 0.0f)
		};
		glm::vec2 uv_coords[4] = {
			{side_uv.uv[0].x, side_uv.uv[0].y},
			{side_uv.uv[1].x, side_uv.uv[1].y},
			{side_uv.uv[2].x, side_uv.uv[0].y + (side_uv.uv[2].y - side_uv.uv[0].y) * height},
			{side_uv.uv[3].x, side_uv.uv[0].y + (side_uv.uv[3].y - side_uv.uv[0].y) * height}
		};
		for (int i = 0; i < 4; i++) {
			mesh->addVertex(verts[i], { -1, 0, 0 }, uv_coords[i]);
		}
		mesh->addTriangle(vertex_count + 0, vertex_count + 1, vertex_count + 2);
		mesh->addTriangle(vertex_count + 2, vertex_count + 3, vertex_count + 0);
	}
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
	BlockModel neighbor_model = BlocksInfo[neighbor_block].block_model;

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