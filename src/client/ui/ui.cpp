#include "ui.h"
#include "../core/client.h"
#include <core/ecs/core/ecs.h>
#include "../utils/resource/resources.h"
#include <core/logger.hpp>

#include "elements/base_element.h"
#include "elements/text.h"
#include "elements/container.h"

#include <glm/glm.hpp>

#include <memory>
#include <algorithm>

UI::UI(Client* client, Resources* resources) {
	m_client = client;
	m_resources = resources;
}

UI::~UI() {

}

void UI::draw() {
	for (auto& element : m_elements) {
		if (element && element->isVisible()) {
			element->draw();
		}
	}

	m_frame_counter.frame();
}

void UI::update(int mouse_x, int mouse_y) {
	for (auto& element : m_elements) {
		if (element && element->isVisible()) {
			element->update(mouse_x, mouse_y);
		}
	}
}

void UI::updateScreenSize(const int width, const int height) {
	m_screen_width = width;
	m_screen_height = height;
	for (auto& element : m_elements) {
		element->setProjection(width, height); 
	}
}

void UI::addElement(std::unique_ptr<UIElement> element) {
	if (!element) {
		LOG_WARN("Attempted to add null element to UI");
		return;
	}

	if (!element->getId().empty()) {
		if (getElementById(element->getId()) != nullptr) {
			LOG_WARN("UI element with ID '{}' already exists", element->getId());
		}
	}

	m_elements.push_back(std::move(element));
}

UIElement* UI::getElementById(const std::string& id) {
	if (id.empty()) return nullptr;
	for (auto& element : m_elements) {
		if (element->getId() == id) return element.get();

		if (auto container = dynamic_cast<UIContainer*>(element.get())) {
			UIElement* found = container->findChildById(id);
			if (found) return found;
		}
	}
	return nullptr;
}

bool UI::removeElement(const std::string& id) {
	if (id.empty()) {
		return false;
	}

	auto it = std::remove_if(m_elements.begin(), m_elements.end(),
		[&id](const std::unique_ptr<UIElement>& element) {
			return element && element->getId() == id;
		});

	if (it != m_elements.end()) {
		m_elements.erase(it, m_elements.end());
		return true;
	}
	return false;
}

UIElement* UI::getElement(size_t index) {
	if (index < m_elements.size()) {
		return m_elements[index].get();
	}
	return nullptr;
}