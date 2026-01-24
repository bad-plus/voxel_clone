#pragma once

struct Collider {
	float half_x = 1.0f;
	float half_y = 1.0f;
	float half_z = 1.0f;

	operator glm::vec3() const {
		return glm::vec3(half_x, half_y, half_z);
	}
};