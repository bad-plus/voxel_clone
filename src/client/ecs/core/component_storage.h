#pragma once
#include "entity.h"

#include <unordered_map>

template <typename T>
class ComponentStorage {
public:
	std::unordered_map<Entity, T> m_data;

	inline void add(Entity e, const T& c) {
		m_data[e] = c;
	}

	inline bool has(Entity e) const {
		return m_data.find(e) != m_data.end();
	}

	inline T& get(Entity e) {
		return m_data.at(e);
	}

	inline void remove(Entity e) {
		m_data.erase(e);
	}
};