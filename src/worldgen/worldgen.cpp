#include <cstdint>
#include <worldgen.hpp>
#include <stdint.h>
#include "stb_image_write.h"
#include <vector>
#include <PerlinNoise.hpp>

PixelColor RED(255, 0, 0);
PixelColor GREEN(0, 255, 0);
PixelColor BLUE(0, 0, 255);

Noise::Noise(seed_t _seed, double _frequency, uint32_t _octaves, uint64_t size_x, uint64_t size_y) {
    seed = _seed;
    frequency = _frequency;
    octaves = _octaves;

    fx = frequency / size_x;
    fy = frequency / size_y;
}

float Noise::get2D(uint64_t x, uint64_t y) {
    return noise.octave2D_01(x * fx, y * fy, octaves);
}

Terrain::Terrain(uint64_t _size_x, uint64_t _size_z, seed_t _masterSeed) {
    size_x = _size_x;
    size_z = _size_z;
    masterSeed = _masterSeed;
    std::srand(masterSeed);
    seed_t cSeed  = std::rand();
    seed_t eSeed  = std::rand();
    seed_t pvSeed = std::rand();

    cNoise   = Noise(cSeed,  cSeed  % 20, cSeed  % 10, size_x, size_z);
    eNoise   = Noise(eSeed,  eSeed  % 20, eSeed  % 10, size_x, size_z);
    pvNoise  = Noise(pvSeed, pvSeed % 20, pvSeed % 10, size_x, size_z);
}

float Terrain::getHeight(uint64_t x, uint64_t z) {
    float cValue  = cNoise.get2D(x, z);
    float eValue  = eNoise.get2D(x, z);
    float pvValue = pvNoise.get2D(x, z);
    
    float finalValue;
    if (cValue < 0.3)
        finalValue = 0.1 * cValue;
    else if (cValue < 0.6)
        finalValue = 0.8 * cValue + 0.3 * 0.1;
    else
        finalValue = 0.05 * cValue + 0.8 * 0.6;

    return finalValue;
}

std::vector<std::vector<float>> Terrain::genMap() {
    std::vector<std::vector<float>> heights;
    uint8_t pixels[size_x * size_z * 3];
    for (int i = 0; i < size_z; i++) {
        std::vector<float> row;
        for (int j = 0; j < size_x; j++) {
            float noise = getHeight(i, j);
            row.push_back(noise);
            int offset = 3 * (size_x * i + j);
            pixels[offset] = pixels[offset + 1] = pixels[offset + 2] = static_cast<int>(noise * 255);
        }
        heights.push_back(row);
    }
    stbi_write_png("terrainMap.png", size_x, size_z, 3, pixels, size_x * 3);
    return heights;
}
