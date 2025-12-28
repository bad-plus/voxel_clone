#include "../mesh_generator.h"

void MeshGenerator::generateLayerModel(
	Mesh* mesh,
	BlockID block_id,
	BlockType block_type,
	glm::ivec3 block_pos,
	const BlockTexture& texture,
	uint8_t layer_count) {

	const BlockInfo& info = GetBlockInfo(block_id);
	float height = layer_count * info.layer_height;

	if (height >= 1.0f) {
		generateCubeModel(mesh, block_id, block_type, block_pos, texture);
		return;
	}

	SideUV top_uv = texture.sides[std::to_underlying(BlockSide::TOP)];
	SideUV bottom_uv = texture.sides[std::to_underlying(BlockSide::BOTTOM)];
	SideUV side_uv = texture.sides[std::to_underlying(BlockSide::FRONT)];

	GLuint vertex_count = static_cast<GLuint>(mesh->getVertexCount());
	glm::vec3 base_pos = glm::vec3(block_pos);

	// TOP
	{
		glm::vec3 verts[4] = {
			base_pos + glm::vec3(0.0f, height, 1.0f),
			base_pos + glm::vec3(1.0f, height, 1.0f),
			base_pos + glm::vec3(1.0f, height, 0.0f),
			base_pos + glm::vec3(0.0f, height, 0.0f)
		};

		std::array<uint8_t, 4> aos = calculateFaceAO(block_pos, BlockSide::TOP);

		for (int i = 0; i < 4; i++) {
			mesh->addVertex(verts[i], { 0, 1, 0 }, { top_uv.uv[i].x, top_uv.uv[i].y }, Light(), aos[i]);
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

		std::array<uint8_t, 4> aos = calculateFaceAO(block_pos, BlockSide::BOTTOM);

		for (int i = 0; i < 4; i++) {
			mesh->addVertex(verts[i], { 0, -1, 0 }, { bottom_uv.uv[i].x, bottom_uv.uv[i].y }, Light(), aos[i]);
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
		vertex_count += 4;
	}

	// FRONT
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

		std::array<uint8_t, 4> aos = calculateFaceAO(block_pos, BlockSide::FRONT);

		for (int i = 0; i < 4; i++) {
			mesh->addVertex(verts[i], { 0, 0, 1 }, uv_coords[i], Light(), aos[i]);
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
		vertex_count += 4;
	}

	// BACK
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

		std::array<uint8_t, 4> aos = calculateFaceAO(block_pos, BlockSide::BACK);

		for (int i = 0; i < 4; i++) {
			mesh->addVertex(verts[i], { 0, 0, -1 }, uv_coords[i], Light(), aos[i]);
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
		vertex_count += 4;
	}

	// RIGHT
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

		std::array<uint8_t, 4> aos = calculateFaceAO(block_pos, BlockSide::RIGHT);

		for (int i = 0; i < 4; i++) {
			mesh->addVertex(verts[i], { 1, 0, 0 }, uv_coords[i], Light(), aos[i]);
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
		vertex_count += 4;
	}

	// LEFT
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

		std::array<uint8_t, 4> aos = calculateFaceAO(block_pos, BlockSide::LEFT);

		for (int i = 0; i < 4; i++) {
			mesh->addVertex(verts[i], { -1, 0, 0 }, uv_coords[i], Light(), aos[i]);
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
}