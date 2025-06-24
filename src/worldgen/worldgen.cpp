#include "chunk_utility.h"
#include <cstdint>
#include <worldgen.hpp>
#include <stb_image_write.h>
#include <Noise.hpp>

 seed_t WorldGen::masterSeed;
 Noise WorldGen::cNoise;
 Noise WorldGen::eNoise;
 Noise WorldGen::pvNoise;

 unsigned int WorldGen::cWeight  = 9;
 unsigned int WorldGen::eWeight  = 5;
 unsigned int WorldGen::pvWeight = 7;

void WorldGen::setMasterSeed(seed_t _masterSeed) {
    // Sets the master seed and seeds the different nosie maps
    masterSeed = _masterSeed;

    std::srand(masterSeed);
    seed_t cSeed  = std::rand();
    seed_t eSeed  = std::rand();
    seed_t pvSeed = std::rand();

    cNoise   = Noise(cSeed,  cSeed  % 20, cSeed  % 10, BLOCKS_PER_REGION_SIDE, BLOCKS_PER_REGION_SIDE);
    eNoise   = Noise(eSeed,  eSeed  % 5, eSeed  % 10, BLOCKS_PER_REGION_SIDE, BLOCKS_PER_REGION_SIDE);
    pvNoise  = Noise(pvSeed, pvSeed % 20, pvSeed % 10, BLOCKS_PER_REGION_SIDE * 2, BLOCKS_PER_REGION_SIDE * 2);
}

// Splines add more randomness to perlin noise
float cSpline(float cValue) {
    if (cValue < 0.3)
        return 0.1 * cValue;
    else if (cValue < 0.6)
        return 0.8 * cValue + 0.3 * 0.1;
    else
        return 0.05 * cValue + 0.8 * 0.6;
}

float eSpline(float eValue) {
    return eValue;
}

float pvSpline(float pvValue) {
    return pvValue;
}

float WorldGen::getHeight(long x, long z) {
    // Returns the final height of (x, z) coords by sampling the noise maps
    // and feeding them to the spline functions
    // The output of spline functions is merged together by a weighted average
    float cValue  = cSpline(cNoise.get2D(x, z));
    float eValue  = eSpline(eNoise.get2D(x, z));
    float pvValue = pvSpline(pvNoise.get2D(x, z));

    float finalValue = (cValue * cWeight + eValue * eWeight + pvValue * pvWeight) / (cWeight + eWeight + pvWeight);
    // finalValue = cValue;
    return finalValue;
}

void WorldGen::generateRegion(RHeightMap* regionHM, RegionData* regionData) {
    long xoffset = regionHM->rx * BLOCKS_PER_REGION_SIDE;
    long zoffset = regionHM->rz * BLOCKS_PER_REGION_SIDE;
    blockData Stone; Stone.id = 1;
    blockData Air; Air.id = 0;

    for (long x = 0; x < BLOCKS_PER_REGION_SIDE; x++) {
        for (long z = 0; z < BLOCKS_PER_REGION_SIDE; z++) {
            float noise = getHeight(xoffset + x, zoffset + z);
            regionHM->heights[x][z] = noise;
            long chunkX = x / BLOCK_X_SIZE;
            long chunkZ = z / BLOCK_Z_SIZE;
            long localX = x % BLOCK_X_SIZE;
            long localZ = z % BLOCK_Z_SIZE;
            int surfaceHeight = static_cast<int>(noise * BLOCK_Y_SIZE);
            for (long y = 0; y < BLOCK_Y_SIZE; y++)
                regionData->chunks[chunkX][chunkZ].blocks[y][localX][localZ] = y > surfaceHeight ? Air : Stone;
        }
    }
}

void setPixel(uint8_t* pixel, float noise) {
    //pixel[0] = pixel[1] = pixel[2] = static_cast<uint8_t>(noise * 255);
    if (noise >= 0.75)
        pixel[0] = pixel[1] = pixel[2] = 255;
    else if (noise >= 0.45) {
        pixel[0] = pixel[2] = 0;
        pixel[1] = 100;
    }
    else if (noise >= 0.25) {
        pixel[0] = pixel[2] = 0;
        pixel[1] = 255;
    }
    else {
        pixel[0] = pixel[1] = 0;
        pixel[2] = 255;
    }
}

void WorldGen::genImage(const RHeightMap* region, const std::string mapFile) {
    uint8_t pixels[BLOCKS_PER_REGION_SIDE * BLOCKS_PER_REGION_SIDE * 3];
    for (int i = 0; i < BLOCKS_PER_REGION_SIDE; i++)
        for (int j = 0; j < BLOCKS_PER_REGION_SIDE; j++)
            setPixel(pixels + 3 * (BLOCKS_PER_REGION_SIDE * i + j), region->heights[i][j]);
    stbi_write_png(mapFile.c_str(), BLOCKS_PER_REGION_SIDE, BLOCKS_PER_REGION_SIDE, 3, pixels, BLOCKS_PER_REGION_SIDE * 3);
}
