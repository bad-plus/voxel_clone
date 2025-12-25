#include "mesh.h"

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices) {
	m_vertices.insert(m_vertices.end(), vertices.begin(), vertices.end());
	m_indices.insert(m_indices.end(), indices.begin(), indices.end());
}

Mesh::Mesh(const std::vector<GLfloat>& raw_vertices, const std::vector<GLuint>& indices) {
	size_t vertex_count = raw_vertices.size() / 8;
	m_vertices.reserve(vertex_count);

	for (size_t i = 0; i < vertex_count; i++) {
		Vertex vertex;
		size_t offset = i * 8;

		vertex.position.x = raw_vertices[offset + 0];
		vertex.position.y = raw_vertices[offset + 1];
		vertex.position.z = raw_vertices[offset + 2];

		vertex.normal.x = raw_vertices[offset + 3];
		vertex.normal.y = raw_vertices[offset + 4];
		vertex.normal.z = raw_vertices[offset + 5];

		vertex.tex_coord.x = raw_vertices[offset + 6];
		vertex.tex_coord.y = raw_vertices[offset + 7];

		m_vertices.push_back(vertex);
	}

	m_indices.insert(m_indices.end(), indices.begin(), indices.end());
}

void Mesh::addVertex(const Vertex& vertex) {
	m_vertices.push_back(vertex);
}

void Mesh::addVertex(glm::vec3 position, glm::vec3 normal, glm::vec2 tex_coord) {
	addVertex({ position, normal, tex_coord });
}

void Mesh::addTriangle(GLuint i0, GLuint i1, GLuint i2) {
	m_indices.insert(m_indices.end(), { i0, i1, i2 });
}

std::vector<GLfloat> Mesh::getRawVertices() const {
	std::vector<GLfloat> vertices;
	vertices.reserve(m_vertices.size() * 8);

	for (const auto& vertex : m_vertices) {
		vertices.push_back(vertex.position.x);
		vertices.push_back(vertex.position.y);
		vertices.push_back(vertex.position.z);

		vertices.push_back(vertex.normal.x);
		vertices.push_back(vertex.normal.y);
		vertices.push_back(vertex.normal.z);

		vertices.push_back(vertex.tex_coord.x);
		vertices.push_back(vertex.tex_coord.y);
	}

	return vertices;
}

void Mesh::clear() {
	m_vertices.clear();
	m_indices.clear();
}