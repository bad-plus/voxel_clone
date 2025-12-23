#pragma once
#include <glm/glm.hpp>
#include <memory>
#include <string>

#include "../../utils/resource/font.h"
#include "base_element.h"
#include "../../render/graphics/text.h"

class UIText : public UIElement {
public:
	UIText(const std::string& str, const int x, const int y, const glm::vec3& color, Font* font, Shader* shader);

	void draw() override;

	void setString(const std::string& str);
	const std::string& getString() const;

	void setColor(const glm::vec3& color);
	const glm::vec3& getColor() const;

	void setScale(const float scale);
	float getScale() const;

	void setPosition(int x, int y);

	void setProjection(const int screen_width, const int screen_height) override;
	void setShader(Shader* shader);

	void setLineSpacing(float spacing);

private:
	std::unique_ptr<Text> m_text;
	std::string m_current_string;
	glm::vec3 m_current_color;
	float m_current_scale = 1.0f;
};