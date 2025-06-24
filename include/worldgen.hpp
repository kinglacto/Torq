#pragma once

#include <stdint.h>
#include <chunk_utility.h>
#include <chunk_utility.h>
#include <Noise.hpp>

struct RHeightMap {
    long rx, rz;
    float heights[CHUNKS_PER_REGION_SIDE * BLOCK_X_SIZE][CHUNKS_PER_REGION_SIDE * BLOCK_Z_SIZE];
    RHeightMap(long _rx, long _rz) : rx(_rx), rz(_rz) {}
};

class WorldGen {
public:
    static seed_t masterSeed;

    static Noise cNoise;  // Continentalness
    static Noise eNoise;  // Erosion
    static Noise pvNoise; // Peaks & Valleys
    
    static unsigned int cWeight;
    static unsigned int eWeight;
    static unsigned int pvWeight;

    static void setMasterSeed(seed_t _masterSeed);
    static float getHeight(long x, long z);
    static void generateRegion(RHeightMap* regionHM, RegionData* regionData);
    static void genImage(const RHeightMap* region, const std::string mapFile);

    static void getChunkData(ChunkData* chunk);
};
