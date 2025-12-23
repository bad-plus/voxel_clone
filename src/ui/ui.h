#pragma once
#include <memory>
#include <vector>
#include <string>
#include "elements/base_element.h"
#include "frame_counter.h"

struct ECS;
struct Resources;
class UIText;

class UI {
public:
	UI(ECS* ecs, Resources* resources);
	~UI();

	void draw();
	void update(int mouse_x, int mouse_y);
	void updateScreenSize(const int width, const int height);

	void addElement(std::unique_ptr<UIElement> element);

	UIElement* getElementById(const std::string& id);

	template<typename T>
	T* getElementById(const std::string& id) {
		return dynamic_cast<T*>(getElementById(id));
	}

	bool removeElement(const std::string& id);

	UIElement* getElement(size_t index);
	size_t getElementCount() const { return m_elements.size(); }

	int getFPS() const { return m_frame_counter.getFPS(); }

private:
	std::vector<std::unique_ptr<UIElement>> m_elements;

	ECS* m_ecs;
	Resources* m_resources;

	int m_screen_width = 800;
	int m_screen_height = 600;

	FrameCounter m_frame_counter;
};