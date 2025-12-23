#include "text.h"

UIText::UIText(const std::string& str, const int x, const int y, const glm::vec3& color, Font* font, Shader* shader)
	: m_current_string(str), m_current_color(color), m_current_scale(1.0f)
{
	m_text = std::make_unique<Text>(font, str, x, y, 1.0f, color, 1, 1);
	setShader(shader);
	setRect(x, y, 0, 0);
}

void UIText::setShader(Shader* shader) {
	if (m_text) {
		m_text->setShader(shader);
	}
}

void UIText::draw() {
	if (!visible || !m_text) return;
	m_text->draw();
}

void UIText::setString(const std::string& str) {
	m_current_string = str;
	if (m_text) {
		m_text->setString(str);
	}
}

const std::string& UIText::getString() const {
	return m_current_string;
}

void UIText::setColor(const glm::vec3& color) {
	m_current_color = color;
	if (m_text) {
		m_text->setColor(color);
	}
}

const glm::vec3& UIText::getColor() const {
	return m_current_color;
}

void UIText::setProjection(const int screen_width, const int screen_height) {
	if (m_text) {
		m_text->setProjection(screen_width, screen_height);
	}
}

void UIText::setScale(const float scale) {
	m_current_scale = scale;
	if (m_text) {
		m_text->setScale(scale);
	}
}

float UIText::getScale() const {
	return m_current_scale;
}

void UIText::setPosition(int x, int y) {
	setRect(x, y, rect.w, rect.h);
	if (m_text) {
		m_text->setPosition(static_cast<float>(x), static_cast<float>(y));
	}
}

void UIText::setLineSpacing(float spacing) {
	if (m_text) {
		m_text->setLineSpacing(spacing);
	}
}