#pragma once
#include <vector>
#include "ecs.h"

template <typename... T>
class View {
public:
	View(ECS& ecs) : m_ecs(ecs) {}

	std::vector<Entity> each() {
		std::vector<Entity> result;

		auto& primary = m_ecs.storage<typename std::tuple_element<0, std::tuple<T...>>::type>().m_data;

		for (auto& pair : primary) {
			Entity e = pair.first;
			if ((m_ecs.storage<T>().has(e) && ...)) {
				result.push_back(e);
			}
		}

		return result;
	}
private:
	ECS& m_ecs;
};