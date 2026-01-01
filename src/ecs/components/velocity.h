#pragma once
#include <glm/glm.hpp>

struct Velocity {
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;

	operator glm::vec3() const {
		return glm::vec3(x, y, z);
	}
};