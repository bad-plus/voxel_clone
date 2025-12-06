#pragma once
#include <vector>

class Perlin2D {
public:
    Perlin2D(int seed);
    ~Perlin2D();

    float noise(float x, float y);
    int get_seed();
private:
    float fade(float t);
    float lerp(float a, float b, float t);
    float grad(int hash, float x, float y);

    int m_seed;
    std::vector<int> m_perm;
};