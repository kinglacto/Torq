#pragma once

#include <cstdint>
#include <stdint.h>
#include <vector>
#include <chunk_utility.h>
#include <PerlinNoise.hpp>

typedef struct PixelColor {
    uint8_t r, g,b;
    PixelColor(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) {}
    PixelColor(uint8_t v) : r(v), g(v), b(v) {}
} PixelColor;
extern PixelColor RED;
extern PixelColor GREEN;
extern PixelColor BLUE;

using seed_t = siv::PerlinNoise::seed_type;

class Noise {
public:
    seed_t seed;
    double frequency;
    double fx, fy;
    uint32_t octaves;
    siv::PerlinNoise noise;

    Noise(seed_t _seed, double _frequency, uint32_t _octaves, uint64_t size_x, uint64_t size_y);
    Noise() = default;
    float get2D(uint64_t x, uint64_t y);
};

class Terrain {
public:
    uint64_t size_x, size_z;
    seed_t masterSeed;
    std::vector<std::vector<float>> heightMap;
    
    Noise cNoise;  // Continentalness
    Noise eNoise;  // Erosion
    Noise pvNoise; // Peaks & Valleys
    
    Terrain(uint64_t _size_x, uint64_t _size_z, seed_t _masterSeed);
    float getHeight(uint64_t x, uint64_t z);
    void genMap();

    void getChunkData(ChunkData* chunk);
};

