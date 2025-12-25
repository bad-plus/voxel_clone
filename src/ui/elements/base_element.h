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

	Rect getRect() const { return m_rect; }

	void setPosition(int x, int y, Anchor anchor = Anchor::TOP_LEFT) {
		m_rect.x = x;
		m_rect.y = y;
		m_anchor = anchor;
	}

	void setSize(int w, int h) { m_rect.w = w; m_rect.h = h; }
	void setId(const std::string& id) { m_id = id; }
	const std::string& getId() const { return m_id; }
	void setVisible(bool visible) { m_visible = visible; }
	bool isVisible() const { return m_visible; }

	virtual void updatePosition(int sw, int sh) {
		switch (m_anchor) {
		case Anchor::TOP_LEFT:     m_rect.screen_x = m_rect.x; m_rect.screen_y = m_rect.y; break;
		case Anchor::TOP_RIGHT:    m_rect.screen_x = sw - m_rect.w - m_rect.x; m_rect.screen_y = m_rect.y; break;
		case Anchor::BOTTOM_LEFT:  m_rect.screen_x = m_rect.x; m_rect.screen_y = sh - m_rect.h - m_rect.y; break;
		case Anchor::BOTTOM_RIGHT: m_rect.screen_x = sw - m_rect.w - m_rect.x; m_rect.screen_y = sh - m_rect.h - m_rect.y; break;
		case Anchor::CENTER:       m_rect.screen_x = (sw - m_rect.w) / 2 + m_rect.x; m_rect.screen_y = (sh - m_rect.h) / 2 + m_rect.y; break;
		}
	}

	virtual void setProjection(const int sw, const int sh) { updatePosition(sw, sh); }
	virtual void update(int mx, int my) {}
	virtual void draw() = 0;

protected:
	std::string m_id;
	Rect m_rect = { 0,0,0,0,0,0 };
	Anchor m_anchor = Anchor::TOP_LEFT;
	bool m_visible = true;
};