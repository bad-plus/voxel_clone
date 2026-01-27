#pragma once
#include "../core/ecs.h"
#include "../core/view.h"
#include "../components/player_camera.h"
#include "../components/player_input.h"
#include <glm/glm.hpp>

class PlayerCameraSystem {
public:
	void update(ECS& ecs) {
		View<Camera, PlayerInput, PlayerControlledCamera> view(ecs);

		for (Entity e : view.each()) {
			auto& cam = ecs.storage<Camera>().get(e);
			const auto& inp = ecs.storage<PlayerInput>().get(e);

			cam.yaw += inp.mouse_delta_x;
			cam.pitch = glm::clamp(cam.pitch - inp.mouse_delta_y, -89.f, 89.f);
			cam.yaw = fmod(cam.yaw, 360.f);

			updateCameraVectors(cam);
		}
	}

private:
	void updateCameraVectors(Camera& cam) {
		glm::vec3 front;
		front.x = cos(glm::radians(cam.yaw)) * cos(glm::radians(cam.pitch));
		front.y = sin(glm::radians(cam.pitch));
		front.z = sin(glm::radians(cam.yaw)) * cos(glm::radians(cam.pitch));

		cam.front = glm::normalize(front);
		cam.right = glm::normalize(glm::cross(cam.front, glm::vec3(0, 1, 0)));
		cam.up = glm::normalize(glm::cross(cam.right, cam.front));
	}
};