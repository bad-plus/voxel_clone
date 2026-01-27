#pragma once
#include "../core/ecs.h"
#include "../core/view.h"
#include "../components/collider.h"
#include "../components/player_input.h"
#include "../components/movement.h"
#include <glm/glm.hpp>
#include <core/constants.h>

class PlayerColliderSystem {
public:
	void update(ECS& ecs, float dt) {
		View<Collider, PlayerInput> view(ecs);

		for (Entity e : view.each()) {
			auto& col = ecs.storage<Collider>().get(e);
			const auto& inp = ecs.storage<PlayerInput>().get(e);
			auto& move = ecs.storage<Movement>().get(e);

			move.target_collider_height = inp.sneak ? Constants::SNEAK_PLAYER_HEIGHT : Constants::DEFAULT_PLAYER_HEIGHT;

			col.half_y = glm::mix(
				col.half_y,
				move.target_collider_height,
				glm::clamp(move.collider_transition_speed * dt, 0.0f, 1.0f)
			);
		}
	}
};