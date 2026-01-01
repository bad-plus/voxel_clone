#pragma once
#include "../core/ecs.h"
#include "../core/view.h"
#include "../components/velocity.h"
#include "../components/player_input.h"
#include "../components/player_mode.h"
#include "../components/player_camera.h"
#include <glm/glm.hpp>

class SpectatorMovementSystem {
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

			if (st.mode != PlayerMode::SPECTATOR) continue;

			glm::vec3 forward = cam.front;
			glm::vec3 forward_flat = glm::normalize(glm::vec3(forward.x, 0.0f, forward.z));
			glm::vec3 right = cam.right;

			float speed = 15.0f * inp.fly_speedup;

			vel.x = forward.x * inp.move_forward * speed + right.x * inp.move_right * speed;
			vel.y = forward.y * inp.move_forward * speed;
			vel.z = forward.z * inp.move_forward * speed + right.z * inp.move_right * speed;

			if (inp.fly_up) vel.y += speed;
			if (inp.fly_down) vel.y -= speed;
		}
	}
};