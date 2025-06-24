#include "PerlinNoise.hpp"
#include <Noise.hpp>
#include <cstdint>

PerlinNoise::PerlinNoise(seed_t _seed, double _frequency, uint32_t _octaves, size_t size_x, size_t size_y) {
    seed = _seed;
    frequency = _frequency;
    octaves = _octaves;
    noise = siv::PerlinNoise { seed };

    fx = frequency / size_x;
    fy = frequency / size_y;
}

float PerlinNoise::get2D(uint64_t x, uint64_t y) {
    return noise.octave2D_01(x * fx, y * fy, octaves);
}

Noise::Noise(int seed, float frequency, FastNoiseLite::NoiseType noiseType) {
    noise = FastNoiseLite(seed);
    noise.SetFrequency(frequency);
    noise.SetNoiseType(noiseType);
}

float Noise::getNoise(float x, float z) {
    return noise.GetNoise(x, z);
}
