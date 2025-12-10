#include "text.h"

#include <glm/gtc/matrix_transform.hpp>

Text::Text(Font* font, const std::string& str, float x, float y, float scale, const glm::vec3& color, int screen_width, int screen_height) {
	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);

	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	setFont(font);
	setString(str);
	setPosition(x, y);
	setScale(scale);
	setColor(color);
	setProjection(screen_width, screen_height);
	setLineSpacing(1.2f);

	m_shader = nullptr;
}

Text::~Text() {
	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(1, &m_VBO);
}

void Text::setProjection(int screen_width, int screen_height) {
	m_projection = glm::ortho(0.0f, (float)screen_width, 0.0f, (float)screen_height);
}

void Text::setString(const std::string& str) {
	m_string = str;
}

void Text::setPosition(float x, float y) {
	m_position = glm::vec2(x, y);
}

void Text::setScale(float scale) {
	m_scale = scale;
}

void Text::setShader(Shader* shader) {
	m_shader = shader;
}

void Text::setColor(const glm::vec3& color) {
	m_color = color;
}

void Text::setFont(Font* font) {
	m_font = font;
}

void Text::setLineSpacing(float spacing) {
	m_lineSpacing = spacing;
}

void Text::draw() {
	if (m_shader == nullptr || m_font == nullptr) return;

	m_shader->use();
	m_shader->uniformVec4("textColor", m_color.r, m_color.g, m_color.b, 1.0f);
	m_shader->uniformmat4fv("projection", m_projection);

	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(m_VAO);

	float x = m_position.x;
	float y = m_position.y;
	float startX = m_position.x;

	float lineHeight = 0.0f;
	for (const auto& pair : m_font->m_glyphs) {
		if (pair.second.size.y > lineHeight) {
			lineHeight = pair.second.size.y;
		}
	}
	lineHeight *= m_scale * m_lineSpacing;

	for (char c : m_string) {
		if (c == '\n') {
			x = startX;
			y -= lineHeight;
			continue;
		}

		if (m_font->m_glyphs.find(c) == m_font->m_glyphs.end()) {
			continue;
		}

		Glyph glyph = m_font->m_glyphs[c];

		float xpos = x + glyph.bearing.x * m_scale;
		float ypos = y - (glyph.size.y - glyph.bearing.y) * m_scale;

		float w = glyph.size.x * m_scale;
		float h = glyph.size.y * m_scale;

		float vertices[6][4] = {
			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos,     ypos,       0.0f, 1.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },

			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },
			{ xpos + w, ypos + h,   1.0f, 0.0f }
		};

		glBindTexture(GL_TEXTURE_2D, glyph.texture);

		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		x += (glyph.advance >> 6) * m_scale;
	}

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}