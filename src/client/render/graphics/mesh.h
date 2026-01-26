#pragma once
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <core/world/lighting/light.h>

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 tex_coord;
	Light light;
	uint8_t ao;


	Vertex() = default;
	Vertex(
		glm::vec3 pos, glm::vec3 norm, glm::vec2 uv, Light light_ = Light(), uint8_t ao_ = 0
	) : position(pos), normal(norm), tex_coord(uv), light(light_), ao(ao_) {

	}
};

class Mesh {
public:
	Mesh() = default;
	Mesh(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices);
	Mesh(const std::vector<GLfloat>& raw_vertices, const std::vector<GLuint>& indices);

	~Mesh() = default;

	void addVertex(const Vertex& vertex);
	void addVertex(glm::vec3 position, glm::vec3 normal, glm::vec2 tex_coord, Light light = Light(), uint8_t ao = 0);

	void addTriangle(GLuint i0, GLuint i1, GLuint i2);

	const std::vector<Vertex>& getVertices() const { return m_vertices; };
	const std::vector<GLuint>& getIndices() const { return m_indices; };

	std::vector<GLfloat> getRawVertices() const;

	size_t getVertexCount() const { return m_vertices.size(); }
	size_t getIndexCount() const { return m_indices.size(); }
	size_t getTriangleCount() const { return m_indices.size() / 3; }

	bool isEmpty() const { return m_vertices.empty() || m_indices.empty(); }
	void clear();

	void reserveVertices(size_t size) { m_vertices.reserve(size); }
	void reserveIndices(size_t size) { m_indices.reserve(size); }
private:
	std::vector<Vertex> m_vertices;
	std::vector<GLuint> m_indices;
};