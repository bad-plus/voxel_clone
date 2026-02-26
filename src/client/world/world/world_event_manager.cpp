#include "world_event_manager.h"

template<typename T>
void WorldEventManager<T>::push(std::unique_ptr<WorldEvent<T>> event, bool priority) {
    std::lock_guard<std::mutex> lock(m_event_list_mutex);
    if (priority) m_events.push_front(std::move(event));
    else m_events.push_back(std::move(event));
}

template<typename T>
void WorldEventManager<T>::process(T& world) {
    size_t eventsToProcess = m_events.size();
    while (eventsToProcess-- > 0) {
        std::unique_ptr<WorldEvent<T>> event;
        {
            std::lock_guard<std::mutex> lock(m_event_list_mutex);
            if (m_events.empty()) break;
            event = std::move(m_events.front());
            m_events.pop_front();
        }
        if (event && !m_exit.load(std::memory_order_relaxed)) {
            event->apply(world, *this);
        }
    }
}

template class WorldEventManager<World>;
template class WorldEventManager<ClientWorld>;
template class WorldEventManager<ServerWorld>;