#pragma once

namespace Constants {
	constexpr unsigned int CHUNK_SIZE_X = 16;
	constexpr unsigned int CHUNK_SIZE_Y = 512; // height
	constexpr unsigned int CHUNK_SIZE_Z = 16;
	constexpr unsigned int CHUNK_SIZE_VOLUME = CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z;
}