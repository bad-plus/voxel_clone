#pragma once
#include "container.h"

class UIVerticalLayout : public UIContainer {
public:
	void updatePosition(int sw, int sh) override {
		int current_y_offset = 0;

		bool grow_upward = (m_anchor == Anchor::BOTTOM_LEFT ||
			m_anchor == Anchor::BOTTOM_RIGHT);

		for (auto& child : m_children) {
			child->updatePosition(sw, sh);

			int child_y = grow_upward
				? rect.screen_y - current_y_offset - child->getRect().h
				: rect.screen_y + current_y_offset;

			child->setPosition(rect.screen_x, child_y, Anchor::TOP_LEFT);
			child->updatePosition(sw, sh);
			current_y_offset += (child->getRect().h + m_spacing);
		}
		rect.h = current_y_offset;

		UIElement::updatePosition(sw, sh);

		current_y_offset = 0;
		for (auto& child : m_children) {
			int child_y = grow_upward
				? rect.screen_y - current_y_offset - child->getRect().h
				: rect.screen_y + current_y_offset;

			child->setPosition(rect.screen_x, child_y, Anchor::TOP_LEFT);
			child->updatePosition(sw, sh);
			current_y_offset += (child->getRect().h + m_spacing);
		}
	}
};