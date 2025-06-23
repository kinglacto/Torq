#pragma once

#include <stdint.h>
#include <vector>
#include <chunk_utility.h>
#include <Noise.hpp>

class Terrain {
public:
    size_t size_x, size_z;
    seed_t masterSeed;
    std::vector<std::vector<float>> heightMap;
    std::string mapFile;
    
    Noise cNoise;  // Continentalness
    Noise eNoise;  // Erosion
    Noise pvNoise; // Peaks & Valleys
    
    Terrain(seed_t _masterSeed);
    float getHeight(size_t x, size_t z);
    void genMap();
    void extendMap(size_t newX, size_t newZ);

    void getChunkData(ChunkData* chunk);
};

