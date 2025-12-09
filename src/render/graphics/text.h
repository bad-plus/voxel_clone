#pragma once
#include <string>
#include <glm/glm.hpp>
#include "../../utils/resource/font.h"
#include "shader.h"

class Text {
public:
	Text(Font* font, const std::string& str, float x, float y, float scale, const glm::vec3& color, int screen_width, int screen_height);
	~Text();

	void setProjection(int screen_width, int screen_height);
	void setString(const std::string& str);
	void setPosition(float x, float y);
	void setScale(float scale);
	void setShader(Shader* shader);
	void setColor(const glm::vec3& color);
	void setFont(Font* font);

	void draw();
private:
	GLuint m_VAO;
	GLuint m_VBO;
	Shader* m_shader;
	Font* m_font;
	float m_scale;
	glm::mat4 m_projection;
	glm::vec3 m_color;
	glm::vec2 m_position;
	std::string m_string;
};