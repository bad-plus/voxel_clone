#pragma once
#include "container.h"

class UIHorizontalLayout : public UIContainer {
public:
	void updatePosition(int sw, int sh) override {
		UIElement::updatePosition(sw, sh);

		int current_x_offset = 0;

		bool grow_leftward = (m_anchor == Anchor::TOP_RIGHT ||
			m_anchor == Anchor::BOTTOM_RIGHT);

		for (auto& child : m_children) {
			int child_x = grow_leftward
				? m_rect.screen_x - current_x_offset - child->getRect().w 
				: m_rect.screen_x + current_x_offset;

			child->setPosition(child_x, m_rect.screen_y, Anchor::TOP_LEFT);
			child->updatePosition(sw, sh);
			current_x_offset += (child->getRect().w + m_spacing);
		}
		m_rect.w = current_x_offset;
	}
};