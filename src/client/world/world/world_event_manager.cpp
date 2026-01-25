#include "world_event_manager.h"

#include <core/logger.hpp>

void WorldEventManager::push(std::unique_ptr<WorldEvent> event, bool priority)
{
	std::lock_guard<std::mutex> lock(m_event_list_mutex);
	if (priority)
		m_events.push_front(std::move(event));
	else
		m_events.push_back(std::move(event));
}

void WorldEventManager::process(World& world)
{
	size_t eventsToProcess = m_events.size();

	while (eventsToProcess-- > 0 && !m_events.empty()) {
		std::unique_ptr<WorldEvent> event;
		{
			std::lock_guard<std::mutex> lock(m_event_list_mutex);
			if (m_events.empty()) break;
			event = std::move(m_events.front());	
			m_events.pop_front();
		}

		if (m_exit.load(std::memory_order_relaxed) && !event->isDontMiss()) {
			continue;
		}

		event->apply(world, *this);
	}
}