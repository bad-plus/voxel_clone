	#pragma once
	#include "../../components/collider.h"
	#include "../../../world/block/block.h"
	#include "../../world/world.h"
	#include "collider_vs_block.h"
	#include <glm/glm.hpp>

	namespace Utils {	
		bool checkCollisionToWorld(World* world, const glm::vec3& position, const Collider& collider) {

			int min_check_x = static_cast<int>(std::floor(position.x - collider.half_x)) - 1;
			int max_check_x = static_cast<int>(std::floor(position.x + collider.half_x)) + 1;
			int min_check_y = static_cast<int>(std::floor(position.y - collider.half_y)) - 1;
			int max_check_y = static_cast<int>(std::floor(position.y + collider.half_y)) + 1;
			int min_check_z = static_cast<int>(std::floor(position.z - collider.half_z)) - 1;
			int max_check_z = static_cast<int>(std::floor(position.z + collider.half_z)) + 1;

			for (int by = min_check_y; by <= max_check_y; by++) {
				for (int bz = min_check_z; bz <= max_check_z; bz++) {
					for (int bx = min_check_x; bx <= max_check_x; bx++) {

						Block* block = world->getBlock(bx, by, bz);
						if (block == nullptr) continue;

						BlockID block_id = block->getBlockID();
						if (block_id == BlockID::EMPTY) continue;

						const BlockInfo& block_info = GetBlockInfo(block_id);
						if (!block_info.block_movement) continue;

						if (Utils::AABBvsBlock(position.x, position.y, position.z, collider, bx, by, bz)) {
							return true;
						}
					}
				}
			}

			return false;
		}
	}