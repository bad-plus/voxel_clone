#include "world_event_manager.h"

#include "../../core/logger.h"

void WorldEventManager::push(std::unique_ptr<WorldEvent> event, bool priority)
{
	if (priority) m_events.push_front(std::move(event));
	else m_events.push_back(std::move(event));
}

void WorldEventManager::process(World& world)
{
	while (!m_events.empty()) {
		auto event = std::move(m_events.front());
		m_events.pop_front();
		event->apply(world, *this);
	}
}
