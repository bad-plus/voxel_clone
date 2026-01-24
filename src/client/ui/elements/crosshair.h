#pragma once
#include <glm/glm.hpp>
#include <memory>
#include <string>

#include "base_element.h"
#include "../../render/graphics/mesh.h"
#include "../../render/graphics/mesh_renderer.h"
#include "../../render/graphics/shader.h"

class UICrosshair : public UIElement {
public:
	UICrosshair(const glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f));

	void draw() override;

	void setColor(const glm::vec3& color);
	const glm::vec3& getColor() const;

	void setScale(const float scale);
	float getScale() const;

	void setShader(Shader* shader) { m_shader = shader; }

	void setProjection(int sw, int sh) override {
		m_screen_w = sw;
		m_screen_h = sh;
	}
private:
	glm::vec3 m_color;
	float m_scale = 1.0f;

	Mesh m_mesh;
	MeshRenderer m_mesh_renderer;
	Shader* m_shader;

	int m_screen_w;
	int m_screen_h;
};