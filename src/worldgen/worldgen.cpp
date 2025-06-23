#include "texture.h"
#include <cstdint>
#include <filesystem>
#include <string>
#include <worldgen.hpp>
#include <stb_image_write.h>
#include <vector>
#include <Noise.hpp>

Terrain::Terrain(seed_t _masterSeed) {
    size_x = 1024;
    size_z = 1024;
    masterSeed = _masterSeed;
    mapFile = std::string(CACHE_DIR) + "/maps/terrain-" + std::to_string(masterSeed) + ".png";
    
    std::srand(masterSeed);
    seed_t cSeed  = std::rand();
    seed_t eSeed  = std::rand();
    seed_t pvSeed = std::rand();

    cNoise   = Noise(cSeed,  cSeed  % 20, cSeed  % 10, size_x, size_z);
    eNoise   = Noise(eSeed,  eSeed  % 20, eSeed  % 10, size_x, size_z);
    pvNoise  = Noise(pvSeed, pvSeed % 20, pvSeed % 10, size_x, size_z);
}

float Terrain::getHeight(size_t x, size_t z) {
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

void setPixel(uint8_t* pixel, float noise) {
    *pixel = *(pixel + 1) = *(pixel + 2) = static_cast<int>(noise * 255);
}

void Terrain::genMap() {
    if (std::filesystem::exists(mapFile)) {
        int width, height, channels;
        uint8_t *pixels = stbi_load(mapFile.c_str(), &width, &height, &channels, 3);
        if (pixels == nullptr) {
            std::cout << "Failed to load " << mapFile << std::endl;
            return;
        }
        heightMap.clear();
        std::vector<float> row;
        for (size_t x = 0; x < width; x++) {
            for (size_t z = 0; z < height; z++) {
                row.push_back(*(pixels + ((z * width) + x) * channels) / 255.0f);
            }
            heightMap.push_back(row);
            row.clear();
        }
        stbi_image_free(pixels);
    } else {
        heightMap.clear();
        uint8_t pixels[size_x * size_z * 3];
        std::vector<float> row;
        for (int i = 0; i < size_x; i++) {
            for (int j = 0; j < size_z; j++) {
                float noise = getHeight(i, j);
                row.push_back(noise);
                setPixel(pixels + 3 * (size_x * i + j), noise);
            }
            heightMap.push_back(row);
            row.clear();
        }
        stbi_write_png(mapFile.c_str(), size_x, size_z, 3, pixels, size_x * 3);
    }
}

void Terrain::extendMap(size_t deltaX, size_t deltaZ) {
    if ((deltaX % CHUNKS_PER_REGION_SIDE != 0) || (deltaZ % CHUNKS_PER_REGION_SIDE != 0))
        return;
    uint8_t pixels[(size_x + deltaX) * (size_z + deltaZ) * 3];
    bool writeToFile = true;
    
    for (size_t x = 0; x < size_x; x++) {
        for (size_t z = size_z; z < size_z + deltaZ; z++) {
            float noise = getHeight(x, z);
            heightMap[x].push_back(noise);
            if (writeToFile)
                setPixel(pixels + 3 * ((size_x + deltaX) * x + z), noise);
        }
    }
    std::vector<float> row;
    for (size_t x = size_x; x < size_x + deltaX; x++) {
        for (size_t z = 0; z < size_z + deltaZ; z++) {
            float noise = getHeight(x, z);
            row.push_back(noise);
            if (writeToFile)
                setPixel(pixels + 3 * ((size_x + deltaX) * x + z), noise);
        }
        heightMap.push_back(row);
        row.clear();
    }
    if (writeToFile) {
        for (size_t x = 0; x < size_x; x++)
            for (size_t z = 0; z < size_z; z++)
                setPixel(pixels + 3 * ((size_x + deltaX) * x + z), heightMap[x][z]);
        stbi_write_png(mapFile.c_str(), size_x + deltaX, size_z + deltaZ, 3, pixels, (size_x + deltaX) * 3);
    }
    size_x += deltaX;
    size_z += deltaZ;
}

void Terrain::getChunkData(ChunkData* chunk) {
    if (heightMap.empty())
        return;
    struct blockData Stone;
    Stone.id = 1;
    struct blockData Air;
    Air.id = 0;
    for (uint32_t x = 0; x < BLOCK_X_SIZE; x++) {
        for (uint32_t y = 0; y < BLOCK_Y_SIZE; y++) {
            for (uint32_t z = 0; z < BLOCK_Z_SIZE; z++) {
                uint32_t surfaceHeight = static_cast<uint32_t>(heightMap[x][z] * BLOCK_Y_SIZE);
                chunk->blocks[y][x][z] = y > surfaceHeight ? Air : Stone;
            }
        }
    }
}