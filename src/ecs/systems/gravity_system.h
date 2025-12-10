#pragma once
#include "../core/ecs.h"
#include "../core/view.h"
#include "../components/transform.h"
#include "../components/velocity.h"
#include "../components/mass.h"
#include <glm/glm.hpp>
#include <cmath>

constexpr float GRAVITY = 16.0f;

class GravitySystem {
public:
	void update(ECS& ecs, float delta_time) {
		View <Transform, Velocity, Mass> view(ecs);

		for (Entity e : view.each()) {
			auto& trans = ecs.storage<Transform>().get(e);
			auto& vel = ecs.storage<Velocity>().get(e);
			auto& mass = ecs.storage<Mass>().get(e);

			if (vel.y <= 40 && vel.y < 0) {
				vel.y -= mass.value * GRAVITY * delta_time * 2.0f;
			}
			else {
				vel.y -= mass.value * GRAVITY * delta_time;
			}

		}
	}
};