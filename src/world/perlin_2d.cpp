#include "perlin_2d.h"
#include <cmath>
#include <random>
#include <algorithm>

Perlin2D::Perlin2D(int seed) {
    m_seed = seed;

    m_perm.resize(512);
    std::vector<int> p(256);

    for(int i = 0; i < 256; i++)
        p[i] = i;

    std::mt19937 rng(seed);
    std::shuffle(p.begin(), p.end(), rng);

    for(int i = 0; i < 512; i++)
        m_perm[i] = p[i & 255];
}
Perlin2D::~Perlin2D() {

}

float Perlin2D::fade(float t) {
    return t * t * t * (t * (t * 6 - 15) + 10);
}
float Perlin2D::lerp(float a, float b, float t) {
    return a + t * (b - a);
}
float Perlin2D::grad(int hash, float x, float y) {
    int h = hash & 3;
    float u = h < 2 ? x : y;
    float v = h < 2 ? y : x;
    return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
}

float Perlin2D::noise(float x, float y) {
    int X = (int)floor(x) & 255;
    int Y = (int)floor(y) & 255;

    float xf = x - floor(x);
    float yf = y - floor(y);

    float u = fade(xf);
    float v = fade(yf);

    int aa = m_perm[X + m_perm[Y]];
    int ab = m_perm[X + m_perm[Y + 1]];
    int ba = m_perm[X + 1 + m_perm[Y]];
    int bb = m_perm[X + 1 + m_perm[Y + 1]];

    float x1 = lerp(grad(aa, xf, yf),
                    grad(ba, xf-1.0f, yf), u);

    float x2 = lerp(grad(ab, xf, yf - 1.0f),
                    grad(bb, xf-1.0f, yf-1.0f), u);

    return lerp(x1, x2, v);
}