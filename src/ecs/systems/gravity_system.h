#pragma once
#include "../core/ecs.h"
#include "../core/view.h"
#include "../components/transform.h"
#include "../components/velocity.h"
#include "../components/player_mode.h"
#include "../components/mass.h"
#include <glm/glm.hpp>

constexpr float GRAVITY = 20.0f;
constexpr float TERMINAL_VELOCITY = -50.0f;
constexpr float MASS_MULTIPLAYER = 1.0f;

class GravitySystem {
public:
	void update(ECS& ecs, float delta_time) {
		View<Velocity> view(ecs);

		for (Entity e : view.each()) {
			if (ecs.storage<PlayerState>().has(e)) {
				const auto& state = ecs.storage<PlayerState>().get(e);
				if (state.mode != PlayerMode::SURVIVAL) {
					continue;
				}
			}

			auto& vel = ecs.storage<Velocity>().get(e);

			float mass = 1.0f;

			if (ecs.storage<Mass>().has(e)) {
				mass = ecs.storage<Mass>().get(e).value;
			}

			if (vel.y > TERMINAL_VELOCITY) {
				vel.y -= (MASS_MULTIPLAYER * mass) * GRAVITY * delta_time;
			}
		}
	}
};