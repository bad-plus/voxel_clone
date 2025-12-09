#pragma once
#include <array>

class Perlin2D {
public:
	explicit Perlin2D(int seed);

	float noise(float x, float y) const;
	int get_seed() const { return m_seed; }

private:
	inline float fade(float t) const {
		return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
	}

	inline float lerp(float a, float b, float t) const {
		return a + t * (b - a);
	}

	inline float grad(int hash, float x, float y) const {
		int h = hash & 7;
		float u = h < 4 ? x : y;
		float v = h < 4 ? y : x;
		return ((h & 1) ? -u : u) + ((h & 2) ? -2.0f * v : 2.0f * v);
	}

	int m_seed;
	std::array<uint8_t, 512> m_perm;
};