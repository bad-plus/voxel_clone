#pragma once
#include "base_element.h"
#include <vector>
#include <memory>

class UIContainer : public UIElement {
public:
	void addChild(std::unique_ptr<UIElement> child) {
		m_children.push_back(std::move(child));
	}

	UIElement* findChildById(const std::string& id) {
		for (auto& child : m_children) {
			if (child->getId() == id) return child.get();
			if (auto sub = dynamic_cast<UIContainer*>(child.get())) {
				UIElement* found = sub->findChildById(id);
				if (found) return found;
			}
		}
		return nullptr;
	}

	void draw() override {
		if (!visible) return;
		for (auto& child : m_children) if (child->isVisible()) child->draw();
	}

	void update(int mx, int my) override {
		if (!visible) return;
		for (auto& child : m_children) {
			if (child->isVisible()) child->update(mx, my);
		}
	}

	void setProjection(const int sw, const int sh) override {
		updatePosition(sw, sh);
		for (auto& child : m_children) {
			child->setProjection(sw, sh);
		}
	}

protected:
	std::vector<std::unique_ptr<UIElement>> m_children;
	int m_spacing = 2;
};