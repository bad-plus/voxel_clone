#pragma once

struct Movement {
	float target_velocity_x = 0.0f;
	float target_velocity_z = 0.0f;
	float acceleration = 30.0f;
	float deceleration = 25.0f;
	float air_control = 0.3f;

	float target_collider_height = 0.9f;
	float collider_transition_speed = 8.0f;
};