#pragma once
#include <cmath>
#include <glm/glm.hpp>

struct Camera {
	glm::vec3 position;

	float pitch = 0.0f;
	float yaw = 0.0f;
	float fov = 70.0f;
	float zoom = 45.0f;

	// Cache
	glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
};

struct PlayerControlledCamera {};

struct ActiveCamera {};