#pragma once
#include "../core/ecs.h"
#include "../core/view.h"
#include "../components/player_camera.h"
#include "../components/transform.h"
#include "../components/collider.h"

class CameraUpdateSystem {
public:
	void update(ECS& ecs) {
		View<Camera, Transform> view(ecs);

		for (Entity e : view.each()) {
			auto& cam = ecs.storage<Camera>().get(e);
			const auto& trans = ecs.storage<Transform>().get(e);

			if (ecs.storage<Collider>().has(e)) {
				const auto& col = ecs.storage<Collider>().get(e);
				cam.position = trans.position + glm::vec3(0, col.half_y, 0);
			}
			else {
				cam.position = trans.position;
			}
		}
	}
};