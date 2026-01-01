#pragma once
#include "../core/ecs.h"
#include "../core/view.h"
#include "../components/transform.h"
#include "../components/velocity.h"
#include "../components/collider.h"
#include "../components/player_input.h"
#include "../components/player_mode.h"
#include "utils/collider_vs_world.h"
#include "../../world/block/block.h"
#include "../../world/world.h"
#include <cmath>
#include <glm/glm.hpp>

class WorldCollisionSystem {
public:
	void update(ECS& ecs, float dt, World* world) {
		if (world == nullptr) return;

		View<Transform, Velocity, Collider, PhysicsState> view(ecs);

		for (Entity e : view.each()) {
			auto& trans = ecs.storage<Transform>().get(e);
			auto& vel = ecs.storage<Velocity>().get(e);
			const auto& col = ecs.storage<Collider>().get(e);
			auto& phys = ecs.storage<PhysicsState>().get(e);

			bool has_player_state = ecs.storage<PlayerState>().has(e);
			bool skip_collision = false;
			bool is_survival = false;
			bool wants_jump = false;

			if (has_player_state) {
				const auto& st = ecs.storage<PlayerState>().get(e);

				if (st.mode == PlayerMode::SPECTATOR) {
					skip_collision = true;
				}

				is_survival = (st.mode == PlayerMode::SURVIVAL);

				if (ecs.storage<PlayerInput>().has(e)) {
					const auto& inp = ecs.storage<PlayerInput>().get(e);
					wants_jump = inp.jump;
				}
			}

			if (skip_collision) {
				trans.position.x += vel.x * dt;
				trans.position.y += vel.y * dt;
				trans.position.z += vel.z * dt;
				continue;
			}

			if (Utils::checkCollisionToWorld(world, trans.position, col)) {
				trans.position.y += 1.0f;
				continue;
			}

			float delta_x = (vel.x * dt);
			float delta_y = (vel.y * dt);
			float delta_z = (vel.z * dt);

			// X
			while (delta_x != 0.0f) {
				float current_delta = 0.0f;

				if (delta_x > 1.0f) {
					current_delta = 1.0f;
					delta_x -= 1.0f;
				}
				else {
					current_delta = delta_x;
					delta_x = 0.0f;
				}

				float current_x = trans.position.x + current_delta;

				bool collision_x = Utils::checkCollisionToWorld(world, { current_x, trans.position.y, trans.position.z }, col);
				if (!collision_x) trans.position.x = current_x;
			}
			
			// Y
			bool was_moving_down = delta_y < 0.0f;
			bool hit_ground_this_frame = false;

			while (delta_y != 0.0f) {
				float current_delta = 0.0f;

				if (delta_y > 1.0f) {
					current_delta = 1.0f;
					delta_y -= 1.0f;
				}
				else if (delta_y < -1.0f) {
					current_delta = -1.0f;
					delta_y += 1.0f;
				}
				else {
					current_delta = delta_y;
					delta_y = 0.0f;
				}

				float current_y = trans.position.y + current_delta;

				bool collision_y = Utils::checkCollisionToWorld(world, { trans.position.x, current_y, trans.position.z }, col);
				if (!collision_y) {
					trans.position.y = current_y;
				}
				else {
					if (was_moving_down && current_delta < 0) {
						hit_ground_this_frame = true;
						vel.y = 0.0f;
					}
				}
			}

			if (hit_ground_this_frame) {
				phys.on_ground = true;
			}
			else if (!hit_ground_this_frame && delta_y == 0.0f && vel.y <= 0.0f) {
				phys.on_ground = Utils::checkCollisionToWorld(world,
					{ trans.position.x, trans.position.y - 0.01f, trans.position.z }, col);
			}

			// Z
			while (delta_z != 0.0f) {
				float current_delta = 0.0f;

				if (delta_z > 1.0f) {
					current_delta = 1.0f;
					delta_z -= 1.0f;
				}
				else {
					current_delta = delta_z;
					delta_z = 0.0f;
				}

				float current_z = trans.position.z + current_delta;

				bool collision_z = Utils::checkCollisionToWorld(world, { trans.position.x, trans.position.y, current_z }, col);
				if (!collision_z) trans.position.z = current_z;
			}
		}
	}
};