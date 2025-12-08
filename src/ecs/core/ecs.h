#pragma once
#include "entity.h"
#include "component_storage.h"

#include "../components/transform.h"
#include "../components/velocity.h"
#include "../components/player_camera.h"
#include "../components/player_tag.h"
#include "../components/player_input.h"
#include "../components/player_mode.h"
#include "../components/collider.h"
#include "../components/mass.h"

#include <typeinfo>

class ECS {
public:
	ECS() {
		registerComponent<Transform>();
		registerComponent<Velocity>();
		registerComponent<PlayerCamera>();
		registerComponent<PlayerTag>();
		registerComponent<PlayerInput>();
		registerComponent<PlayerState>();
		registerComponent<Collider>();
		registerComponent<Mass>();
	}
	~ECS() {
		for (auto& pair : m_components) {
			delete static_cast<ComponentStorage<void*>*>(pair.second);
		}
	}

	template <typename T>
	ComponentStorage<T>& storage() {
		return *reinterpret_cast<ComponentStorage<T>*>(m_components[typeid(T).hash_code()]);
	}

	template<typename T>
	void registerComponent() {
		m_components[typeid(T).hash_code()] = new ComponentStorage<T>();
	}

	inline Entity create() {
		return m_next_id++;
	}
private:
	Entity m_next_id = 1;
	std::unordered_map<size_t, void*> m_components;
};