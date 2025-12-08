#pragma once

#include "../core/logger.h"

#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../ecs/core/ecs.h"

inline glm::mat4 getCameraViewMatrix(ECS* ecs, Entity entity) {
	if (!ecs->storage<PlayerInput>().has(entity) ||
		!ecs->storage<PlayerCamera>().has(entity) ||
		!ecs->storage<Transform>().has(entity)) {
		return glm::mat4(1.0f);
	}

	auto& player_input = ecs->storage<PlayerInput>().get(entity);
	auto& player_camera = ecs->storage<PlayerCamera>().get(entity);
	auto& player_transform = ecs->storage<Transform>().get(entity);

	glm::vec3 m_world_up = glm::vec3(0.0f, 1.0f, 0.0f);

	glm::vec3 front;
	front.x = cos(glm::radians(player_camera.yaw)) * cos(glm::radians(player_camera.pitch));
	front.y = sin(glm::radians(player_camera.pitch));
	front.z = sin(glm::radians(player_camera.yaw)) * cos(glm::radians(player_camera.pitch));

	glm::vec3 m_front = glm::normalize(front);
	glm::vec3 m_right = glm::normalize(glm::cross(m_front, m_world_up));
	glm::vec3 m_up = glm::normalize(glm::cross(m_right, m_front));

	return glm::lookAt(player_transform.position, player_transform.position + m_front, m_up);
}