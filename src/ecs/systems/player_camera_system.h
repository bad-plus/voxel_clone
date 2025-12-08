#pragma once

#include "../core/ecs.h"
#include "../core/view.h"
#include "../components/player_camera.h"
#include "../components/player_input.h"

class CameraSystem {
public:
	void update(ECS& ecs, float dt) {
		View<PlayerCamera, PlayerInput> view(ecs);

		for (Entity e : view.each()) {
			auto& cam = ecs.storage<PlayerCamera>().get(e);
			auto& inp = ecs.storage<PlayerInput>().get(e);

			cam.yaw += inp.mouse_delta_x;
			cam.pitch -= inp.mouse_delta_y;

			if (cam.pitch > 89.f) cam.pitch = 89.f;
			if (cam.pitch < -89.f) cam.pitch = -89.f;
		}
	}
};