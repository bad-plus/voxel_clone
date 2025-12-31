#pragma once
#include <memory>
#include <deque>
#include "world_event.h"
#include "../world.h"

class WorldEventManager {
public:
	WorldEventManager() = default;
	~WorldEventManager() = default;

	void push(std::unique_ptr<WorldEvent> event, bool priority = false);
	void process(World& world);
	void shutdown() { m_exit = true; }
private:
	bool m_exit = false;

	std::deque<std::unique_ptr<WorldEvent>> m_events;
};