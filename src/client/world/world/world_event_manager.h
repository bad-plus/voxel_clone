#pragma once
#include <memory>
#include <deque>
#include <mutex>
#include "world_event.h"

class World;
class ClientWorld;
class ServerWorld;

template <typename T>
class WorldEventManager {
public:
	WorldEventManager() = default;
	~WorldEventManager() = default;

	void push(std::unique_ptr<WorldEvent<T>> event, bool priority = false);
	void process(T& world);
	void shutdown() {
		m_exit.store(true, std::memory_order_relaxed);
	}

private:
	std::atomic<bool> m_exit{ false };	
	std::deque<std::unique_ptr<WorldEvent<T>>> m_events;
	mutable std::mutex m_event_list_mutex;
};