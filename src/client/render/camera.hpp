#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <core/ecs/core/ecs.h>
#include <core/ecs/components/player_camera.h>

inline glm::mat4 getCameraViewMatrix(ECS* ecs, Entity entity) {
	if (!ecs->storage<Camera>().has(entity)) {
		return glm::mat4(1.0f);
	}

	const auto& cam = ecs->storage<Camera>().get(entity);

	return glm::lookAt(cam.position, cam.position + cam.front, cam.up);
}

inline glm::vec3 getCameraFront(ECS* ecs, Entity entity) {
	if (!ecs->storage<Camera>().has(entity)) {
		return glm::vec3(0.0f, 0.0f, -1.0f);
	}

	const auto& cam = ecs->storage<Camera>().get(entity);
	return cam.front;
}