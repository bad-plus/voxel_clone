#include "mesh_renderer.h"

MeshRenderer::MeshRenderer() {
	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_EBO);

	setupVertexAttributes();
}
MeshRenderer::~MeshRenderer() {
	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(1, &m_VBO);
	glDeleteBuffers(1, &m_EBO);
}

void MeshRenderer::uploadMesh(const Mesh& mesh) {
	if (mesh.isEmpty()) {
		m_index_count = 0;
		return;
	}

	std::vector<GLfloat> raw_vertices = mesh.getRawVertices();

	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, raw_vertices.size() * sizeof(GLfloat), raw_vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.getIndexCount() * sizeof(GLuint), mesh.getIndices().data(), GL_STATIC_DRAW);

	glBindVertexArray(0);

	m_index_count = mesh.getIndexCount();
}

void MeshRenderer::draw() const {
	if (!hasData()) return;

	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, m_index_count, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void MeshRenderer::setupVertexAttributes() const {
	constexpr GLsizei VERTEX_SIZE = ((8 * sizeof(GLfloat)) + (2 * sizeof(GL_UNSIGNED_BYTE)));

	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTEX_SIZE, (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, VERTEX_SIZE, (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// Texture coordinate attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, VERTEX_SIZE, (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	// Light attribute
	glVertexAttribPointer(3, 1, GL_UNSIGNED_BYTE, GL_FALSE, VERTEX_SIZE, (GLvoid*)(8 * sizeof(GLfloat)));
	glEnableVertexAttribArray(3);

	// AO attribute
	glVertexAttribPointer(4, 1, GL_UNSIGNED_BYTE, GL_FALSE, VERTEX_SIZE, (GLvoid*)(6 * sizeof(GLfloat) + sizeof(GL_UNSIGNED_BYTE)));
	glEnableVertexAttribArray(4);

	glBindVertexArray(0);
}