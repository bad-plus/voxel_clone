#pragma once
#include <string>

enum class Anchor {
	TOP_LEFT, TOP_RIGHT, BOTTOM_LEFT, BOTTOM_RIGHT, CENTER
};

struct Rect {
	int x, y;
	int w, h;
	int screen_x, screen_y;
};

class UIElement {
public:
	virtual ~UIElement() = default;

	Rect getRect() const { return rect; }

	void setPosition(int x, int y, Anchor anchor = Anchor::TOP_LEFT) {
		rect.x = x;
		rect.y = y;
		m_anchor = anchor;
	}

	void setSize(int w, int h) { rect.w = w; rect.h = h; }
	void setId(const std::string& id) { m_id = id; }
	const std::string& getId() const { return m_id; }
	void setVisible(bool visible) { this->visible = visible; }
	bool isVisible() const { return visible; }

	virtual void updatePosition(int sw, int sh) {
		switch (m_anchor) {
		case Anchor::TOP_LEFT:     rect.screen_x = rect.x; rect.screen_y = rect.y; break;
		case Anchor::TOP_RIGHT:    rect.screen_x = sw - rect.w - rect.x; rect.screen_y = rect.y; break;
		case Anchor::BOTTOM_LEFT:  rect.screen_x = rect.x; rect.screen_y = sh - rect.h - rect.y; break;
		case Anchor::BOTTOM_RIGHT: rect.screen_x = sw - rect.w - rect.x; rect.screen_y = sh - rect.h - rect.y; break;
		case Anchor::CENTER:       rect.screen_x = (sw - rect.w) / 2 + rect.x; rect.screen_y = (sh - rect.h) / 2 + rect.y; break;
		}
	}

	virtual void setProjection(const int sw, const int sh) { updatePosition(sw, sh); }
	virtual void update(int mx, int my) {}
	virtual void draw() = 0;

protected:
	std::string m_id;
	Rect rect = { 0,0,0,0,0,0 };
	Anchor m_anchor = Anchor::TOP_LEFT;
	bool visible = true;
};