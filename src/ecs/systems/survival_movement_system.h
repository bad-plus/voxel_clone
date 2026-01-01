#pragma once
#include "../core/ecs.h"
#include "../core/view.h"
#include "../components/velocity.h"
#include "../components/player_input.h"
#include "../components/player_mode.h"
#include "../components/player_camera.h"
#include <glm/glm.hpp>

constexpr float SURVIVAL_PLAYER_SPEED = 8.0f;

class SurvivalMovementSystem {
public:
	void update(ECS& ecs) const {
		View <Transform, Velocity, Camera, PlayerInput, PlayerState, PhysicsState> view(ecs);

		for (Entity e : view.each()) {
			auto& vel = ecs.storage<Velocity>().get(e);
			auto& inp = ecs.storage<PlayerInput>().get(e);
			auto& cam = ecs.storage<Camera>().get(e);
			auto& st = ecs.storage<PlayerState>().get(e);
			auto& phys = ecs.storage<PhysicsState>().get(e);
			auto& trans = ecs.storage<Transform>().get(e);

			if (st.mode != PlayerMode::SURVIVAL) continue;

			glm::vec3 forward = cam.front;
			glm::vec3 forward_flat = glm::normalize(glm::vec3(forward.x, 0.0f, forward.z));
			glm::vec3 right = cam.right;

			float speed = SURVIVAL_PLAYER_SPEED;

			if (inp.jump && phys.on_ground) {
				vel.y = 7.0f;
				phys.on_ground = false;
			}

			vel.x = (forward_flat.x * inp.move_forward + right.x * inp.move_right) * speed;
			vel.z = (forward_flat.z * inp.move_forward + right.z * inp.move_right) * speed;
		}
	}
};