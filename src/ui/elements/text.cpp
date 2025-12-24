#include "text.h"
#include "../../core/logger.h"

UIText::UIText(const std::string& str, const int x, const int y, const glm::vec3& color, Font* font, Shader* shader)
	: m_current_string(str), m_current_color(color), m_current_scale(1.0f)
{
	m_text = std::make_unique<Text>(font, str, 0.0f, 0.0f, 1.0f, color, 1, 1);

	setShader(shader);
	rect.x = x;
	rect.y = y;
	rect.w = 0;
	rect.h = 0;

	updateTextSize();
}
void UIText::updatePosition(int sw, int sh) {
	UIElement::updatePosition(sw, sh);
	if (m_text) {
		m_text->setProjection(sw, sh);

		Font* font = m_text->getFont();
		float font_height = 0.0f;

		if (font) {
			if (font->m_glyphs.count('H')) {
				font_height = font->m_glyphs.at('H').size.y * m_current_scale;
			}
			else if (!font->m_glyphs.empty()) {
				font_height = font->m_glyphs.begin()->second.size.y * m_current_scale;
			}
		}

		float opengl_y = static_cast<float>(rect.screen_y) + font_height;

		m_text->setPosition(static_cast<float>(rect.screen_x), opengl_y);
	}
}

void UIText::setPosition(int x, int y, Anchor anchor) {
	rect.x = x;
	rect.y = y;
	m_anchor = anchor;
}

void UIText::draw() {
	if (!visible || !m_text) return;

	m_text->draw();
}

void UIText::setString(const std::string& str) {
	m_current_string = str;
	if (m_text) {
		m_text->setString(str);
		updateTextSize();
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

void UIText::setScale(const float scale) {
	m_current_scale = scale;
	if (m_text) {
		m_text->setScale(scale);
	}
}

float UIText::getScale() const {
	return m_current_scale;
}

void UIText::setShader(Shader* shader) {
	if (m_text) {
		m_text->setShader(shader);
	}
}

void UIText::setLineSpacing(float spacing) {
	
}

void UIText::updateTextSize() {
	if (!m_text || m_current_string.empty()) {
		rect.w = 0;
		rect.h = 20;
		return;
	}

	Font* font = m_text->getFont();
	if (!font) return;

	float total_width = 0;
	float max_height = 0;

	for (char c : m_current_string) {
		if (c == '\n') break;

		if (font->m_glyphs.find(c) != font->m_glyphs.end()) {
			const Glyph& glyph = font->m_glyphs[c];
			total_width += (glyph.advance >> 6) * m_current_scale;

			float glyph_height = glyph.size.y * m_current_scale;
			if (glyph_height > max_height) {
				max_height = glyph_height;
			}
		}
	}

	rect.w = static_cast<int>(total_width) + 5;
	rect.h = static_cast<int>(max_height) + 2;
}