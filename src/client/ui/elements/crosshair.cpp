#include "crosshair.h"

UICrosshair::UICrosshair(const glm::vec3 color) {
	m_color = color;
	m_scale = 1.0f;

	m_screen_w = 800;
	m_screen_h = 600;

	glm::vec3 crosshair_normal = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec2 crosshair_uv = glm::vec2(0.0f, 0.0f);

	const float line_length = 0.03f;
	const float line_width = 0.005f;

	// horizontal
	m_mesh.addVertex({ -line_length,  line_width, 0.0f }, crosshair_normal, crosshair_uv);
	m_mesh.addVertex({ line_length,  line_width, 0.0f }, crosshair_normal, crosshair_uv);
	m_mesh.addVertex({ -line_length, -line_width, 0.0f }, crosshair_normal, crosshair_uv);
	m_mesh.addVertex({ line_length, -line_width, 0.0f }, crosshair_normal, crosshair_uv);

	// vertical
	m_mesh.addVertex({ -line_width,  line_length, 0.0f }, crosshair_normal, crosshair_uv);
	m_mesh.addVertex({ line_width,  line_length, 0.0f }, crosshair_normal, crosshair_uv);
	m_mesh.addVertex({ -line_width, -line_length, 0.0f }, crosshair_normal, crosshair_uv);
	m_mesh.addVertex({ line_width, -line_length, 0.0f }, crosshair_normal, crosshair_uv);

	// horizontal
	m_mesh.addTriangle(0, 2, 1);
	m_mesh.addTriangle(1, 2, 3);

	// vertical
	m_mesh.addTriangle(4, 6, 5);
	m_mesh.addTriangle(5, 6, 7);

	m_mesh_renderer.uploadMesh(m_mesh);
}

void UICrosshair::draw() {
	if (!m_shader) return;

	m_shader->use();

	glEnable(GL_BLEND);

	glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);

	float aspect = (m_screen_h > 0) ? (float)m_screen_w / (float)m_screen_h : 1.0f;

	m_shader->uniformVec4("uColor", 1.0f, 1.0f, 1.0f, 1.0f);
	m_shader->uniform1f("uScale", m_scale);
	m_shader->uniform1f("uAspect", aspect);

	m_mesh_renderer.draw();

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void UICrosshair::setColor(const glm::vec3& color) {
	m_color = color;
}
const glm::vec3& UICrosshair::getColor() const {
	return m_color;
}

void UICrosshair::setScale(const float scale) {
	m_scale = scale;
}
float UICrosshair::getScale() const {
	return m_scale;
}