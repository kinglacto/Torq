#include <Noise.hpp>
#include <cstdint>

Noise::Noise(seed_t _seed, double _frequency, uint32_t _octaves, size_t size_x, size_t size_y) {
    seed = _seed;
    frequency = _frequency;
    octaves = _octaves;

    fx = frequency / size_x;
    fy = frequency / size_y;
}

float Noise::get2D(uint64_t x, uint64_t y) {
    return noise.octave2D_01(x * fx, y * fy, octaves);
}
