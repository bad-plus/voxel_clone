#pragma once
#include "../../components/collider.h"
#include <core/world/block/block.h>
#include "../../world/world.h"

namespace Utils {
	inline bool AABBvsBlock(float x, float y, float z, const Collider& collider, int bx, int by, int bz) {
		float min_x = x - collider.half_x;
		float max_x = x + collider.half_x;
		float min_y = y - collider.half_y;
		float max_y = y + collider.half_y;
		float min_z = z - collider.half_z;
		float max_z = z + collider.half_z;

		return (max_x > bx && min_x < bx + 1 &&
			max_y > by && min_y < by + 1 &&
			max_z > bz && min_z < bz + 1);
	}
}