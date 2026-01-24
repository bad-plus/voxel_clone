#pragma once
#include "mesh.h"

class MeshRenderer {
public:
	MeshRenderer();
	~MeshRenderer();

	MeshRenderer(const MeshRenderer&) = delete;
	MeshRenderer& operator=(const MeshRenderer&) = delete;

	void uploadMesh(const Mesh& mesh);

	void draw() const;

	bool hasData() const { return m_index_count > 0; }

	GLuint getVAO() const { return m_VAO; }
private:
	void setupVertexAttributes() const;

	GLuint m_VAO;
	GLuint m_VBO;
	GLuint m_EBO;

	GLuint m_index_count;
};