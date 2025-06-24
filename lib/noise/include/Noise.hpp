#pragma once

#include "PerlinNoise.hpp"
#include "FastNoiseLite.h"

using seed_t = siv::PerlinNoise::seed_type;

class PerlinNoise {
public:
    seed_t seed;
    double frequency;
    double fx, fy;
    uint32_t octaves;
    siv::PerlinNoise noise;

    PerlinNoise(seed_t _seed, double _frequency, uint32_t _octaves, size_t size_x, size_t size_y);
    PerlinNoise() = default;
    float get2D(size_t x, size_t y);
};

class Noise {
public:
    FastNoiseLite noise;
    Noise(int seed = 1337, float frequency = 0.01f, FastNoiseLite::NoiseType noiseType = FastNoiseLite::NoiseType::NoiseType_Perlin);
    float getNoise(float x, float z);
};

