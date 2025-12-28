#include "../mesh_generator.h"

void MeshGenerator::generateCrossModel(
	Mesh* mesh,
	glm::ivec3 block_pos,
	const BlockTexture& texture) {

	SideUV face_uv = texture.sides[0];
	GLuint vertex_count = static_cast<GLuint>(mesh->getVertexCount());

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
		mesh->addVertex(quad1_verts[i], normal1, { face_uv.uv[i].x, face_uv.uv[i].y }, Light(), 3);
	}
	mesh->addTriangle(vertex_count + 0, vertex_count + 1, vertex_count + 2);
	mesh->addTriangle(vertex_count + 2, vertex_count + 3, vertex_count + 0);
	mesh->addTriangle(vertex_count + 2, vertex_count + 1, vertex_count + 0);
	mesh->addTriangle(vertex_count + 0, vertex_count + 3, vertex_count + 2);

	vertex_count = static_cast<GLuint>(mesh->getVertexCount());

	for (int i = 0; i < 4; i++) {
		mesh->addVertex(quad2_verts[i], normal2, { face_uv.uv[i].x, face_uv.uv[i].y }, Light(), 3);
	}
	mesh->addTriangle(vertex_count + 0, vertex_count + 1, vertex_count + 2);
	mesh->addTriangle(vertex_count + 2, vertex_count + 3, vertex_count + 0);
	mesh->addTriangle(vertex_count + 2, vertex_count + 1, vertex_count + 0);
	mesh->addTriangle(vertex_count + 0, vertex_count + 3, vertex_count + 2);
}
