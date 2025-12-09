#pragma once
#include <map>
#include <string>
#include <glm/glm.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <glad/glad.h>

struct Glyph {
	GLuint texture;
	glm::ivec2 size;
	glm::ivec2 bearing;
	long advance;
};

class Font {
public:
	std::map<char, Glyph> m_glyphs;

	Font(const std::string& path, int pixel_size);
	~Font();
private:
	FT_Library m_ft;
	FT_Face m_face;
};