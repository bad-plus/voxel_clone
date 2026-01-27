#pragma once
#include "../core/ecs.h"
#include "../core/view.h"
#include "../components/transform.h"
#include "../components/velocity.h"
#include "../components/collider.h"
#include "../components/player_input.h"
#include "../components/player_mode.h"
#include "utils/collider_vs_world.h"
#include <core/world/block/block.h>
#include <cmath>
#include <glm/glm.hpp>

struct World;

class WorldCollisionSystem {
private:
	const float CHECK_MOVE_STEP = 1.0f;

	bool handleStuckInBlock(World* world, glm::vec3& position, const Collider& collider) {
		if (Utils::checkCollisionToWorld(world, position, collider)) {
			position.y += 1.0f;
			return true;
		}
		return false;
	}

	void handleAxisMovement(
		World* world,
		glm::vec3& position,
		glm::vec3& velocity,
		const Collider& collider,
		PhysicsState* phys,
		float& delta,
		char axis) {

		bool was_moving_down = (axis == 'y' && delta < 0.0f);
		bool hit_ground_this_frame = false;

		while (delta != 0.0f) {
			float current_delta = 0.0f;

			if (std::abs(delta) > CHECK_MOVE_STEP) {
				current_delta = (delta > 0) ? CHECK_MOVE_STEP : -CHECK_MOVE_STEP;
				delta -= current_delta;
			}
			else {
				current_delta = delta;
				delta = 0.0f;
			}

			glm::vec3 test_pos = position;

			switch (axis) {
			case 'x': test_pos.x += current_delta; break;
			case 'y': test_pos.y += current_delta; break;
			case 'z': test_pos.z += current_delta; break;
			}

			bool collision = Utils::checkCollisionToWorld(world, test_pos, collider);

			if (!collision) {
				switch (axis) {
				case 'x': position.x = test_pos.x; break;
				case 'y': position.y = test_pos.y; break;
				case 'z': position.z = test_pos.z; break;
				}
			}
			else {
				switch (axis) {
				case 'x': velocity.x = 0.0f; break;
				case 'y':
					velocity.y = 0.0f;
					if (was_moving_down && current_delta < 0) {
						hit_ground_this_frame = true;
					}
					break;
				case 'z': velocity.z = 0.0f; break;
				}
				break;
			}
		}

		if (axis == 'y' && phys != nullptr) {
			if (hit_ground_this_frame) {
				phys->on_ground = true;
			}
			else if (delta == 0.0f && velocity.y <= 0.0f) {
				glm::vec3 ground_check_pos = {
					position.x,
					position.y - 0.01f,
					position.z
				};
				phys->on_ground = Utils::checkCollisionToWorld(world, ground_check_pos, collider);
			}
		}
	}

public:
	void update(ECS& ecs, float dt, World* world) {
		if (world == nullptr) return;

		View<Transform, Velocity, Collider, PhysicsState> view(ecs);

		for (Entity e : view.each()) {
			auto& trans = ecs.storage<Transform>().get(e);
			auto& vel = ecs.storage<Velocity>().get(e);
			const auto& col = ecs.storage<Collider>().get(e);
			auto& phys = ecs.storage<PhysicsState>().get(e);

			glm::vec3 vel_vec = glm::vec3(vel.x, vel.y, vel.z);

			bool has_player_state = ecs.storage<PlayerState>().has(e);
			bool skip_collision = false;
			bool is_survival = false;

			if (has_player_state) {
				const auto& st = ecs.storage<PlayerState>().get(e);

				if (st.mode == PlayerMode::SPECTATOR) {
					skip_collision = true;
				}

				is_survival = (st.mode == PlayerMode::SURVIVAL);

				if (ecs.storage<PlayerInput>().has(e)) {
					const auto& inp = ecs.storage<PlayerInput>().get(e);
				}
			}

			if (skip_collision) {
				trans.position.x += vel.x * dt;
				trans.position.y += vel.y * dt;
				trans.position.z += vel.z * dt;
				continue;
			}

			if (handleStuckInBlock(world, trans.position, col)) {
				continue;
			}

			float delta_x = vel.x * dt;
			float delta_y = vel.y * dt;
			float delta_z = vel.z * dt;

			handleAxisMovement(world, trans.position, vel_vec, col, &phys, delta_x, 'x');
			handleAxisMovement(world, trans.position, vel_vec, col, &phys, delta_z, 'z');
			handleAxisMovement(world, trans.position, vel_vec, col, &phys, delta_y, 'y');

			vel.x = vel_vec.x;
			vel.y = vel_vec.y;
			vel.z = vel_vec.z;
		}
	}
};