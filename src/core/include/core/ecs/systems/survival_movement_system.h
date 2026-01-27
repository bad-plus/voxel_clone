#pragma once
#include "../core/ecs.h"
#include "../core/view.h"
#include "../components/velocity.h"
#include "../components/player_input.h"
#include "../components/player_mode.h"
#include "../components/player_camera.h"
#include "../components/movement.h"
#include <glm/glm.hpp>

constexpr float SURVIVAL_PLAYER_SPEED = 8.0f;

class SurvivalMovementSystem {
public:
	void update(ECS& ecs, float dt) const {
		View <Transform, Velocity, Camera, PlayerInput, PlayerState, PhysicsState> view(ecs);

		for (Entity e : view.each()) {
			auto& vel = ecs.storage<Velocity>().get(e);
			auto& inp = ecs.storage<PlayerInput>().get(e);
			auto& cam = ecs.storage<Camera>().get(e);
			auto& st = ecs.storage<PlayerState>().get(e);
			auto& phys = ecs.storage<PhysicsState>().get(e);
			auto& trans = ecs.storage<Transform>().get(e);
			auto& move = ecs.storage<Movement>().get(e);

			if (st.mode != PlayerMode::SURVIVAL) continue;

			glm::vec3 forward = cam.front;
			glm::vec3 forward_flat = glm::normalize(glm::vec3(forward.x, 0.0f, forward.z));
			glm::vec3 right = cam.right;

			float speed = SURVIVAL_PLAYER_SPEED;

			move.target_velocity_x = (forward_flat.x * inp.move_forward + right.x * inp.move_right) * speed;
			move.target_velocity_z = (forward_flat.z * inp.move_forward + right.z * inp.move_right) * speed;

			bool is_moving = (glm::abs(move.target_velocity_x) > 0.01f || glm::abs(move.target_velocity_z) > 0.01f);
			float rate = is_moving ? move.acceleration : move.deceleration;

			float control = phys.on_ground ? 1.0f : move.air_control;

			vel.x = glm::mix(vel.x, move.target_velocity_x, glm::clamp(rate * control * dt, 0.0f, 1.0f));
			vel.z = glm::mix(vel.z, move.target_velocity_z, glm::clamp(rate * control * dt, 0.0f, 1.0f));

			if (inp.jump && phys.on_ground) {
				vel.y = 7.0f;
				phys.on_ground = false;
			}
		}
	}
};