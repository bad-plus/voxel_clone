#pragma once

struct PlayerInput {
	float move_forward = 0.0f; // W / S
	float move_right = 0.0f; // A / D

	bool jump = false;
	bool sneak = false;

	bool fly_up = false;
	bool fly_down = false;
	float fly_speedup = 1.0f;

	float mouse_delta_x = 0.0f;
	float mouse_delta_y = 0.0f;
};