#include "world_event_manager.h"

#include "../../core/logger.h"	

void WorldEventManager::push(std::unique_ptr<WorldEvent> event, bool priority)
{
	if (priority) m_events.push_front(std::move(event));
	else m_events.push_back(std::move(event));
}

void WorldEventManager::process(World& world)
{
	size_t eventsToProcess = m_events.size();

	while (eventsToProcess-- > 0 && !m_events.empty()) {
		auto event = std::move(m_events.front());
		m_events.pop_front();

		if (m_exit && !event->isDontMiss()) {
			continue;
		}

		event->apply(world, *this);
	}
}