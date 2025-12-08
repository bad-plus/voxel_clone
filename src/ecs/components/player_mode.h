#pragma once

enum class PlayerMode {
	SURVIVAL,
	CREATIVE,
	SPECTATOR
};

struct PlayerState {
	PlayerMode mode = PlayerMode::CREATIVE;
};