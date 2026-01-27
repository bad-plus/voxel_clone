#pragma once

namespace Constants {
	// Player
	constexpr float DEFAULT_PLAYER_HEIGHT = 0.9f;
	constexpr float SNEAK_PLAYER_HEIGHT = 0.8f;

	constexpr float SURVIVAL_PLAYER_SPEED = 8.0f;
	constexpr float CREATIVE_PLAYER_SPEED = 15.0f;
	constexpr float SPECTATOR_PLAYER_SPEED = 20.0f;
	constexpr float MIN_SPECTATOR_PLAYER_SPEED_MULTIPLIER = 0.1f;
	constexpr float MAX_SPECTATOR_PLAYER_SPEED_MULTIPLIER = 10.0f;

	// Chunk
	constexpr unsigned int CHUNK_SIZE_X = 16;
	constexpr unsigned int CHUNK_SIZE_Y = 512; // height
	constexpr unsigned int CHUNK_SIZE_Z = 16;
	constexpr unsigned int CHUNK_SIZE_VOLUME = CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z;

	// Server
	constexpr uint16_t	DEFAULT_SERVER_PORT = 25417;
	constexpr size_t	DEFAULT_MAX_CLIENTS = 32;
}