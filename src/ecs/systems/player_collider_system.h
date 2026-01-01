#pragma once
#include "../core/ecs.h"
#include "../core/view.h"
#include "../components/collider.h"
#include "../components/player_input.h"
#include <glm/glm.hpp>

constexpr float DEFAULT_PLAYER_HEIGHT = 0.9f;
constexpr float SNEAK_PLAYER_HEIGHT = 0.8f;

class PlayerColliderSystem {
public:
	void update(ECS& ecs) {
		View<Collider, PlayerInput> view(ecs);

		for (Entity e : view.each()) {
			auto& col = ecs.storage<Collider>().get(e);
			const auto& inp = ecs.storage<PlayerInput>().get(e);

			if (inp.sneak) {
				col.half_y = SNEAK_PLAYER_HEIGHT;
			}
			else {
				col.half_y = DEFAULT_PLAYER_HEIGHT;
			}
		}
	}
};