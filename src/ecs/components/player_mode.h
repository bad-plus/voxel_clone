#pragma once

enum class PlayerMode {
	SURVIVAL,
	CREATIVE
};

struct PlayerState {
	PlayerMode mode = PlayerMode::CREATIVE;
};