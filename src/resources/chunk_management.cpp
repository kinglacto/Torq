#include <chunk_management.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include <cmath>
#include <zlib.h>

namespace fs = std::filesystem;

ChunkManager::ChunkManager(const std::string& chunkDir){
    header_size = CHUNKS_PER_REGION_SIDE * CHUNKS_PER_REGION_SIDE * sizeof(HeaderEntry);
    setChunkDir(chunkDir);
}

ChunkErrorCode ChunkManager::createRegionFile(int region_x, int region_z){
    std::string filePath = getRegionFileName(region_x, region_z);

    std::ofstream file(filePath, std::ios::binary | std::ios::out);
    if (!file) {
        std::cerr << "Failed to create region file: " << filePath << "\n";
        return FILE_ERROR;
    }

    std::vector<char> zeroes(header_size, 0);

    file.write(zeroes.data(), static_cast<std::streamsize>(header_size));
    if (!file) {
        std::cerr << "Failed to write header to " << filePath << "\n";
        return FILE_ERROR;
    }

    return NO_ERROR;
}

ChunkErrorCode ChunkManager::cacheRegionFile(int region_x, int region_z){
    // NOTE: For now, we rely on Mr.Trovald's page caching by warming up the required file
    // This obviously exists in brevity; isn't meant to be used as it is redundant
    // To be replaced by a robust custom file caching system

    std::string filePath = getRegionFileName(region_x, region_z);
    std::ifstream file(filePath, std::ios::binary);
    if (!file){
        return FILE_ERROR;
    }

    file.seekg(0, std::ios::end);
    std::streampos fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> dummyBuffer(fileSize);
    file.read(dummyBuffer.data(), fileSize);

    return NO_ERROR;
}

ChunkErrorCode ChunkManager::loadChunkMesh(int chunk_x, int chunk_z, bool setActive){
    bool convertStaticToActive = false;
    std::pair<int, int> chunk_pair = {chunk_x, chunk_z};
    auto activePtr = activeChunkMeshes.find(chunk_pair);
    auto staticPtr = staticChunkMeshes.find(chunk_pair);

    if (setActive){
        if (activePtr != activeChunkMeshes.end()){
            return NO_ERROR;
        }

        if (staticPtr != staticChunkMeshes.end()){
            convertStaticToActive = true;
        }
    }

    else {
        if (activePtr != activeChunkMeshes.end()){
            staticPtr->second = std::move(activePtr->second);
            activeChunkMeshes.erase(activePtr);
            staticPtr->second->generateMesh();
            staticPtr->second->isActive = false;
            staticPtr->second->chunk.reset();
            return NO_ERROR;
        }

        if (staticPtr != staticChunkMeshes.end()){
            return NO_ERROR;
        }
    }

    int region_x = static_cast<int>(std::floor(static_cast<float>(chunk_x) / CHUNKS_PER_REGION_SIDE));
    int region_z = static_cast<int>(std::floor(static_cast<float>(chunk_z) / CHUNKS_PER_REGION_SIDE));

    if (regionFileCache.find({region_x, region_z}) == regionFileCache.end()){
        ChunkErrorCode e = createRegionFile(region_x, region_z);
        if (e != NO_ERROR) return e;
        regionFileCache[{region_x, region_z}] = true;
    }

    std::string filePath = getRegionFileName(region_x, region_z);

    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    if (!file){
        std::cerr << "Failed to open file " << filePath << std::endl;
        return FILE_ERROR;
    }

    auto fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> headerBuffer(header_size);
    if (!file.read(reinterpret_cast<char*>(headerBuffer.data()), header_size)){
        std::cerr << "Failed to read the header in: " << filePath << std::endl;
        return HEADER_CORRUPTED;
    }

    auto* header = reinterpret_cast<HeaderEntry*>(headerBuffer.data());

    int chunk_x_index = MathMod(chunk_x, CHUNKS_PER_REGION_SIDE);
    int chunk_z_index = MathMod(chunk_z, CHUNKS_PER_REGION_SIDE);

    auto& entry = header[chunk_x_index * CHUNKS_PER_REGION_SIDE + chunk_z_index];
    size_t offset = static_cast<size_t>(entry.offset);
    size_t length = static_cast<size_t>(entry.length);

    if (offset == 0 || length == 0) {
        std::cout << "Chunk (" << chunk_x_index << ", " << chunk_z_index << ") not found in region file: ("
        << region_x << ", " << region_z << ") . Please write it." << std::endl;
        return CHUNK_EMPTY;
    }

    if (offset < header_size) {
        std::cerr << "Chunk in " << filePath << " has corrupted offset pointing into header.\n";
        return CHUNK_CORRUPTED;
    }


    std::vector<uint8_t> compressedChunkBuffer(length);
    file.seekg(static_cast<std::streamoff>(offset), std::ios::beg);

    if (!file.read(reinterpret_cast<char*>(compressedChunkBuffer.data()), length)){
        std::cerr << "Failed to read the chunk data in: " << filePath << std::endl;
        return FILE_ERROR;
    }

    auto chunk = std::make_unique<Chunk>();
    uLongf destLen = sizeof(chunk->blocks);
    int zret = uncompress(
        reinterpret_cast<Bytef*>(chunk.get()->blocks),
        &destLen,
        compressedChunkBuffer.data(),
        static_cast<uLong>(length)
    );
    if (zret != Z_OK || destLen != sizeof(chunk->blocks)) {
        std::cerr << "Decompression failed\n";
        return COMPRESSION_ERROR;
    }

    chunk->x = chunk_x; 
    chunk->z = chunk_z;

    if (!convertStaticToActive){
        auto chunkMesh = std::make_unique<ChunkMesh>(chunk_x, chunk_z);

        if (setActive){
            chunkMesh->isActive = true;
            chunkMesh->chunk = std::move(chunk);
            activeChunkMeshes[{chunk_x, chunk_z}] = std::move(chunkMesh);
        }

        else{
            staticChunkMeshes[{chunk_x, chunk_z}] = std::move(chunkMesh);
        }
        chunkMesh->generateMesh(chunk.get());
    }

    else{
        staticPtr->second->chunk = std::move(chunk);
        staticPtr->second->isActive = true;
    }

    
    return NO_ERROR;
}

ChunkErrorCode ChunkManager::setChunkDir(const std::string& chunkDir){
    if (fs::exists(chunkDir)){
        ChunkManager::chunkDir = chunkDir;
        return NO_ERROR;
    }

    else{
        std::error_code ec;
        if (fs::create_directories(chunkDir, ec)) {
            ChunkManager::chunkDir = chunkDir;
            return NO_ERROR;
        } else {
            std::cerr << "Failed to create directory: " << chunkDir << "\n"
                      << "Reason: " << ec.message() << std::endl;
            return DIRECTORY_CREATION_FAILED;
        }
    }
}

ChunkMesh* ChunkManager::getChunkMesh(int chunk_x, int chunk_z){
    std::pair<int, int> chunk_pair = {chunk_x, chunk_z};
    auto activePtr = activeChunkMeshes.find(chunk_pair);
    if (activePtr != activeChunkMeshes.end()) {
        return activePtr->second.get();
    }

    auto staticPtr = staticChunkMeshes.find(chunk_pair);
    if (staticPtr != staticChunkMeshes.end()) {
        return staticPtr->second.get();
    }

    return nullptr;
}

ChunkErrorCode ChunkManager::deleteChunkMesh(int chunk_x, int chunk_z){
    // write the chunk to disk before deleting from active list

    std::pair<int, int> chunk_pair = {chunk_x, chunk_z};
    auto activePtr = activeChunkMeshes.find(chunk_pair);
    if (activePtr != activeChunkMeshes.end()) {
        if (activePtr->second->chunkEverUpdated){
            // WRITE BACK
        }
        activePtr->second->deleteMesh();
        activePtr->second.reset();
        activeChunkMeshes.erase(activePtr);
        return NO_ERROR;
    }

    auto staticPtr = staticChunkMeshes.find(chunk_pair);
    if (staticPtr != staticChunkMeshes.end()) {
        if (staticPtr->second->chunkEverUpdated){
            // WRITE BACK
        }
        staticPtr->second->deleteMesh();
        staticPtr->second.reset();
        staticChunkMeshes.erase(staticPtr);
        return NO_ERROR;
    }

    return CHUNK_NOT_FOUND;
}

ChunkMesh* ChunkManager::getActiveChunkMesh(int chunk_x, int chunk_z){
    std::pair<int, int> chunk_pair = {chunk_x, chunk_z};
    auto activePtr = activeChunkMeshes.find(chunk_pair);
    if (activePtr != activeChunkMeshes.end()) {
        return activePtr->second.get();
    }

    return nullptr;
}

ChunkErrorCode ChunkManager::deleteActiveChunkMesh(int chunk_x, int chunk_z){
    // write the chunk to disk before deleting from active list

    std::pair<int, int> chunk_pair = {chunk_x, chunk_z};
    auto activePtr = activeChunkMeshes.find(chunk_pair);
    if (activePtr != activeChunkMeshes.end()) {
        if (activePtr->second->chunkEverUpdated){
            // WRITE BACK
        }
        activePtr->second->deleteMesh();
        activePtr->second.reset();
        activeChunkMeshes.erase(activePtr);
        return NO_ERROR;
    }

    return CHUNK_NOT_FOUND;

}

ChunkMesh* ChunkManager::getStaticChunkMesh(int chunk_x, int chunk_z){
    std::pair<int, int> chunk_pair = {chunk_x, chunk_z};
    auto staticPtr = staticChunkMeshes.find(chunk_pair);
    if (staticPtr != staticChunkMeshes.end()) {
        return staticPtr->second.get();
    }

    return nullptr;
}

ChunkErrorCode ChunkManager::deleteStaticChunkMesh(int chunk_x, int chunk_z){
    // write the chunk to disk before deleting from active list

    std::pair<int, int> chunk_pair = {chunk_x, chunk_z};

    auto staticPtr = staticChunkMeshes.find(chunk_pair);
    if (staticPtr != staticChunkMeshes.end()) {
        if (staticPtr->second->chunkEverUpdated){
            // WRITE BACK
        }
        staticPtr->second->deleteMesh();
        staticPtr->second.reset();
        staticChunkMeshes.erase(staticPtr);
        return NO_ERROR;
    }

    return CHUNK_NOT_FOUND;
}


std::string ChunkManager::getRegionFileName(int region_x, int region_z) {
    std::string filename = chunkDir + "/r." + std::to_string(region_x) + "." + 
    std::to_string(region_z) + ".region";
    return filename;
}

void ChunkManager::getRegionCoordsFromChunkCoords(int chunk_x, int chunk_z, int* region_x, int* region_z){
    *region_x = static_cast<int>(std::floor(static_cast<float>(chunk_x) / CHUNKS_PER_REGION_SIDE));
    *region_z = static_cast<int>(std::floor(static_cast<float>(chunk_z) / CHUNKS_PER_REGION_SIDE));
}

void ChunkManager::getChunkCoordsFromWorldCoords(int world_x, int world_z, int* chunk_x, int* chunk_z){
    *chunk_x = static_cast<int>(std::floor(static_cast<float>(world_x) / BLOCK_X_SIZE));
    *chunk_z = static_cast<int>(std::floor(static_cast<float>(world_z) / BLOCK_Y_SIZE));
}   

void ChunkManager::getRegionCoordsFromWorldCoords(int world_x, int world_z, int* region_x, int* region_z){
    int chunk_x, chunk_z;
    getChunkCoordsFromWorldCoords(world_x, world_z, &chunk_x, &chunk_z);
    getRegionCoordsFromChunkCoords(chunk_x, chunk_z, region_x, region_z);
}