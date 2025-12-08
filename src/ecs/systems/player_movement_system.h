#pragma once
#include "../core/ecs.h"
#include "../core/view.h"
#include "../components/transform.h"
#include "../components/velocity.h"
#include "../components/player_input.h"
#include "../components/player_mode.h"
#include "../components/player_camera.h"
#include "../../world/world.h"
#include <glm/glm.hpp>
#include <cmath>

class PlayerMovementSystem {
public:
	void update(ECS& ecs, float delta_time, World* world) {
		View <Transform, Velocity, PlayerCamera, PlayerInput, PlayerState> view(ecs);

		for (Entity e : view.each()) {
			auto& trans = ecs.storage<Transform>().get(e);
			auto& vel = ecs.storage<Velocity>().get(e);
			auto& inp = ecs.storage<PlayerInput>().get(e);
			auto& cam = ecs.storage<PlayerCamera>().get(e);
			auto& st = ecs.storage<PlayerState>().get(e);

			glm::vec3 front;
			front.x = cos(glm::radians(cam.yaw)) * cos(glm::radians(cam.pitch));
			front.y = sin(glm::radians(cam.pitch));
			front.z = sin(glm::radians(cam.yaw)) * cos(glm::radians(cam.pitch));

			glm::vec3 forward = glm::normalize(front);
			glm::vec3 forward_flat = glm::normalize(glm::vec3(forward.x, 0.0f, forward.z));
			glm::vec3 world_up = glm::vec3(0.0f, 1.0f, 0.0f);
			glm::vec3 right = glm::normalize(glm::cross(forward_flat, world_up));

			float speed = (st.mode == PlayerMode::SURVIVAL) ? 10.0f : (15.0f * inp.fly_speedup);

			float delta_x = (forward_flat.x * inp.move_forward + right.x * inp.move_right) * speed;
			float delta_z = (forward_flat.z * inp.move_forward + right.z * inp.move_right) * speed;

			if (st.mode == PlayerMode::CREATIVE) {
				float delta_y = 0.0f;

				if (inp.fly_up) delta_y = speed;
				if (inp.fly_down) delta_y = -speed;

				vel.x = delta_x;
				vel.y = delta_y;
				vel.z = delta_z;

				continue;
			}

			vel.x = delta_x;
			vel.z = delta_z;

			if (inp.jump) {
				
			}

			// Gravity

			const float gravity = 9.8f;

			if (vel.y <= 40 && vel.y < 0) {
				vel.y -= gravity * delta_time * 2.0f;
			}
			else {
				vel.y -= gravity * delta_time;
			}

		}
	}
};