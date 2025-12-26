#pragma once
#include "../chunk_storage.h"
#include "light.h"

class ChunkLight {
public:
	void setLight(const int x, const int y, const int z, const Light& light) {
		m_lights[toIndex(x, y, z)] = light;
	}

	Light getLight(const int x, const int y, const int z) const {
		return m_lights[toIndex(x, y, z)];
	}
private:
	std::array<Light, CHUNK_SIZE_VOLUME> m_lights;

	static size_t toIndex(const int x, const int y, const int z) const {
		return x + z * CHUNK_SIZE_X + y * CHUNK_SIZE_X * CHUNK_SIZE_Z;
	}
};