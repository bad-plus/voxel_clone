#pragma once
#include <string>

struct Rect {
	int x, y;
	int w, h;

	bool contains(int px, int py) const {
		return px >= x && px <= x + w &&
			py >= y && py <= y + h;
	}
};

class UIElement {
public:
	virtual ~UIElement() = default;

	void setRect(int x, int y, int w, int h) {
		rect = { x, y, w, h };
	}

	void setId(const std::string& id) { m_id = id; }
	const std::string& getId() const { return m_id; }

	void setVisible(bool visible) { this->visible = visible; }
	bool isVisible() const { return visible; }

	bool isHovered() const { return hovered; }

	virtual void setProjection(const int screen_width, const int screen_height) {};

	virtual void update(int mouse_x, int mouse_y) {
		hovered = rect.contains(mouse_x, mouse_y);
	}

	virtual void draw() = 0;
	virtual void onClick() {}

protected:
	std::string m_id;
	Rect rect;
	bool visible = true;
	bool hovered = false;
};