#include "chunk_utility.h"
#include <cstdint>
#include <worldgen.hpp>
#include <stb_image_write.h>
#include <Noise.hpp>

 seed_t WorldGen::masterSeed;
 Noise WorldGen::cNoise;
 Noise WorldGen::eNoise;
 Noise WorldGen::pvNoise;

void WorldGen::setMasterSeed(seed_t _masterSeed) {
    masterSeed = _masterSeed;

    std::srand(masterSeed);
    seed_t cSeed  = std::rand();
    seed_t eSeed  = std::rand();
    seed_t pvSeed = std::rand();

    cNoise   = Noise(cSeed,  cSeed  % 20, cSeed  % 10, BLOCKS_PER_REGION_SIDE, BLOCKS_PER_REGION_SIDE);
    eNoise   = Noise(eSeed,  eSeed  % 20, eSeed  % 10, BLOCKS_PER_REGION_SIDE, BLOCKS_PER_REGION_SIDE);
    pvNoise  = Noise(pvSeed, pvSeed % 20, pvSeed % 10, BLOCKS_PER_REGION_SIDE, BLOCKS_PER_REGION_SIDE);
}

float WorldGen::getHeight(long x, long z) {
    float cValue  = cNoise.get2D(x, z);
    float eValue  = eNoise.get2D(x, z);
    float pvValue = pvNoise.get2D(x, z);

    float finalValue;
    if (cValue < 0.3f)
        finalValue = 0.1f * cValue;
    else if (cValue < 0.6f)
        finalValue = 0.8f * cValue + 0.3f * 0.1f;
    else
        finalValue = 0.05f * cValue + 0.8f * 0.6f;

    return finalValue;
}

void WorldGen::generateRegion(RHeightMap* regionHM, RegionData* regionData) {
    long xoffset = regionHM->rx * BLOCKS_PER_REGION_SIDE;
    long zoffset = regionHM->rz * BLOCKS_PER_REGION_SIDE;
    blockData Stone{}; Stone.id = 1;
    blockData Air{}; Air.id = 0;

    for (long x = 0; x < BLOCKS_PER_REGION_SIDE; x++) {
        for (long z = 0; z < BLOCKS_PER_REGION_SIDE; z++) {
            float noise = getHeight(xoffset + x, zoffset + z);
            regionHM->heights[x][z] = noise;
            long chunkX = static_cast<int>(std::floor(static_cast<float>(x) / BLOCK_X_SIZE));
            long chunkZ = static_cast<int>(std::floor(static_cast<float>(z) / BLOCK_Z_SIZE));
            long localX = MathMod(x, static_cast<long>(BLOCK_X_SIZE));
            long localZ = MathMod(z, static_cast<long>(BLOCK_Z_SIZE));
            int surfaceHeight = static_cast<int>(noise * BLOCK_Y_SIZE);
            regionData->chunks[chunkX][chunkZ].x = chunkX;
            regionData->chunks[chunkX][chunkZ].z = chunkZ;
            for (long y = 0; y < BLOCK_Y_SIZE; y++)
                regionData->chunks[chunkX][chunkZ].blocks[y][localX][localZ] = y > surfaceHeight ? Air : Stone;
        }
    }
}

void setPixel(uint8_t* pixel, const float noise) {
    *pixel = *(pixel + 1) = *(pixel + 2) = static_cast<int>(noise * 255);
}

void WorldGen::genImage(const RHeightMap& region, const std::string& mapFile) {
    uint8_t pixels[BLOCKS_PER_REGION_SIDE * BLOCKS_PER_REGION_SIDE * 3];
    for (int i = 0; i < BLOCKS_PER_REGION_SIDE; i++)
        for (int j = 0; j < BLOCKS_PER_REGION_SIDE; j++)
            setPixel(pixels + 3 * (BLOCKS_PER_REGION_SIDE * i + j), region.heights[i][j]);
    stbi_write_png(mapFile.c_str(), BLOCKS_PER_REGION_SIDE, BLOCKS_PER_REGION_SIDE, 3, pixels, BLOCKS_PER_REGION_SIDE * 3);
}
