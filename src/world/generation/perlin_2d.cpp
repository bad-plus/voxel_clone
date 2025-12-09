#include "perlin_2d.h"
#include <cmath>
#include <random>
#include <algorithm>
#include <numeric>

Perlin2D::Perlin2D(int seed) : m_seed(seed) {
	std::array<uint8_t, 256> p;
	std::iota(p.begin(), p.end(), 0);

	std::mt19937 rng(m_seed);
	std::shuffle(p.begin(), p.end(), rng);

	for (int i = 0; i < 256; i++) {
		m_perm[i] = p[i];
		m_perm[i + 256] = p[i];
	}

	std::uniform_real_distribution<float> dist(-100000.0f, 100000.0f);
	m_offset_x = dist(rng);
	m_offset_z = dist(rng);
}

float Perlin2D::noise(float x, float y) const {
	x += m_offset_x;
	y += m_offset_z;

	int X = static_cast<int>(std::floor(x)) & 255;
	int Y = static_cast<int>(std::floor(y)) & 255;

	float xf = x - std::floor(x);
	float yf = y - std::floor(y);

	float u = fade(xf);
	float v = fade(yf);

	int aa = m_perm[m_perm[X] + Y];
	int ab = m_perm[m_perm[X] + Y + 1];
	int ba = m_perm[m_perm[X + 1] + Y];
	int bb = m_perm[m_perm[X + 1] + Y + 1];

	float x1 = lerp(grad(aa, xf, yf),
		grad(ba, xf - 1.0f, yf), u);
	float x2 = lerp(grad(ab, xf, yf - 1.0f),
		grad(bb, xf - 1.0f, yf - 1.0f), u);

	return lerp(x1, x2, v);
}