#pragma once
#include "../core/ecs.h"
#include "../core/view.h"
#include "../components/transform.h"
#include "../components/velocity.h"
#include "../components/collider.h"
#include "../components/player_input.h"
#include "../components/player_mode.h"
#include "utils/collider_vs_block.h"
#include "../../world/block/block.h"
#include "../../world/world.h"
#include <cmath>

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

			float new_x = trans.position.x + vel.x * dt;
			float new_y = trans.position.y + vel.y * dt;
			float new_z = trans.position.z + vel.z * dt;

			int min_check_x = (int)std::floor(new_x - col.half_x) - 1;
			int max_check_x = (int)std::floor(new_x + col.half_x) + 1;
			int min_check_y = (int)std::floor(new_y - col.half_y) - 1;
			int max_check_y = (int)std::floor(new_y + col.half_y) + 1;
			int min_check_z = (int)std::floor(new_z - col.half_z) - 1;
			int max_check_z = (int)std::floor(new_z + col.half_z) + 1;

			bool collision_x = false;
			bool collision_y = false;
			bool collision_z = false;
			phys.on_ground = false;


			for (int by = min_check_y; by <= max_check_y; by++) {
				for (int bz = min_check_z; bz <= max_check_z; bz++) {
					for (int bx = min_check_x; bx <= max_check_x; bx++) {
						Block* block = world->getBlock(bx, by, bz);
						if (block == nullptr) continue;

						BlockID block_id = block->getBlockID();
						if (block_id == BlockID::EMPTY) continue;

						const BlockInfo& block_info = GetBlockInfo(block_id);
						if (!block_info.is_solid_surface) continue;

						if (Utils::AABBvsBlock(new_x, trans.position.y, trans.position.z, col, bx, by, bz)) {
							collision_x = true;
							vel.x = 0.0f;
							new_x = trans.position.x;
						}
					}
				}
			}

			for (int bx = min_check_x; bx <= max_check_x; bx++) {
				for (int bz = min_check_z; bz <= max_check_z; bz++) {
					for (int by = min_check_y; by <= max_check_y; by++) {
						Block* block = world->getBlock(bx, by, bz);
						if (block == nullptr) continue;

						BlockID block_id = block->getBlockID();
						if (block_id == BlockID::EMPTY) continue;

						const BlockInfo& block_info = GetBlockInfo(block_id);
						if (!block_info.is_solid_surface) continue;

						if (Utils::AABBvsBlock(new_x, new_y, trans.position.z, col, bx, by, bz)) {
							collision_y = true;

							if (vel.y < 0.0f) {
								phys.on_ground = true;
								trans.position.y = (float)(by + 1) + col.half_y;
								new_y = trans.position.y;
							}
							else if (vel.y > 0.0f) {
								trans.position.y = (float)by - col.half_y;
								new_y = trans.position.y;
							}

							vel.y = 0.0f;
						}
					}
				}
			}

			for (int by = min_check_y; by <= max_check_y; by++) {
				for (int bx = min_check_x; bx <= max_check_x; bx++) {
					for (int bz = min_check_z; bz <= max_check_z; bz++) {
						Block* block = world->getBlock(bx, by, bz);
						if (block == nullptr) continue;

						BlockID block_id = block->getBlockID();
						if (block_id == BlockID::EMPTY) continue;

						const BlockInfo& block_info = GetBlockInfo(block_id);
						if (!block_info.is_solid_surface) continue;

						if (Utils::AABBvsBlock(new_x, new_y, new_z, col, bx, by, bz)) {
							collision_z = true;
							vel.z = 0.0f;
							new_z = trans.position.z;
						}
					}
				}
			}

			trans.position.x = new_x;
			trans.position.y = new_y;
			trans.position.z = new_z;
		}
	}
};