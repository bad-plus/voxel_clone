#include "font.h"
#include "../../core/logger.h"

Font::Font(const std::string& path, int pixel_size) {
	if (FT_Init_FreeType(&m_ft)) {
		throw std::runtime_error("Failed to init freetype");
	}
	const std::string font_base_path = "../resources/fonts/";

	if (FT_New_Face(m_ft, (font_base_path + path).c_str(), 0, &m_face)) {
		throw std::runtime_error("Failed to load font");
	}

	FT_Set_Pixel_Sizes(m_face, 0, pixel_size);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	for (unsigned char c = 0; c < 128; c++) {
		if (FT_Load_Char(m_face, c, FT_LOAD_RENDER)) {
			continue;
		}

		GLuint tex;
		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);
		glTexImage2D(
			GL_TEXTURE_2D, 0, GL_RED,
			m_face->glyph->bitmap.width,
			m_face->glyph->bitmap.rows,
			0, GL_RED, GL_UNSIGNED_BYTE,
			m_face->glyph->bitmap.buffer
		);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		Glyph g = {};
		g.texture = tex;
		g.size = { m_face->glyph->bitmap.width, m_face->glyph->bitmap.rows };
		g.bearing = { m_face->glyph->bitmap_left, m_face->glyph->bitmap_top };
		g.advance = m_face->glyph->advance.x;

		m_glyphs[c] = g;
	}
}

Font::~Font() {
	for (const auto& [c, g] : m_glyphs) {
		glDeleteTextures(1, &g.texture);
	}

	FT_Done_Face(m_face);
	FT_Done_FreeType(m_ft);
}