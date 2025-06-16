#include <chunk_management.h>
#include <iostream>
#include <fstream>
#include <cstring>

namespace fs = std::filesystem;

ChunkManager::ChunkManager(const std::string& chunkDir): chunkDir(chunkDir){}

bool ChunkManager::regionFileExists(int region_x, int region_z){
    if (regionFileCache.find({region_x, region_z}) != regionFileCache.end()){
        return true;
    }

    return false;

}

std::unique_ptr<Chunk> ChunkManager::load(int x, int z){
    int region_x = x/chunks_per_region_side;
    int region_z = z/chunks_per_region_side;

    int chunk_x = x % chunks_per_region_side;
    int chunk_z = z % chunks_per_region_side;
    std::string filePath;
    if (regionFileExists(region_x, region_z)){
        filePath = chunkDir + "c." + std::to_string(region_x) + "." + std::to_string(region_z) + ".chunk";
    }

    else{
        std::ofstream file(filePath);
        if (!file) {
            std::cerr << "Failed to create file: " << filePath << std::endl;
            return nullptr;
        }

        regionFileCache[{region_x, region_z}] = true;
    }

    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    if (!file){
        std::cerr << "Failed to open file " << filePath << std::endl;
        return nullptr;
    }

    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);


    std::vector<uint8_t> buffer(fileSize);
    if (!file.read(reinterpret_cast<char*>(buffer.data()), fileSize))
        std::cerr << "Failed to read " << filePath << std::endl;
        return nullptr;


    size_t header_size = chunks_per_region_side * chunks_per_region_side * 
        (sizeof(chunk_header_offset_type) + sizeof(chunk_header_length_type));

    if (buffer.size() < header_size){
        std::cerr << "Header lookup table too small in " << filePath << "... terminating" << std::endl;
        return nullptr;
    }

    struct HeaderEntry{chunk_header_offset_type offset; chunk_header_length_type length;};
    auto header = reinterpret_cast<HeaderEntry*>(buffer.data());

    auto& entry = header[chunk_x * chunks_per_region_side + chunk_z];
    size_t offset = static_cast<size_t>(entry.offset);
    size_t length = static_cast<size_t>(entry.length);
    size_t payloadStart = header_size + offset;
    if (payloadStart + length > buffer.size()){
        std::cerr << "chunk in " << filePath << " corrupted for chunk: " << chunk_x << ", " <<
        chunk_z << " ... terminating" << std::endl;
        return nullptr;
    }

    uint8_t* payloadPtr = buffer.data() + payloadStart;

    // NOTE: decompress
    if (length != sizeof(Chunk)){
        std::cerr << "Unexpected chunk size: " << length << " ... terminating" << std::endl;
        return nullptr;
    }

    auto chunk = std::make_unique<Chunk>();
    std::memcpy(&chunk, payloadPtr, length);
    return chunk;

}

bool ChunkManager::setChunkDir(const std::string& chunkDir){
    ChunkManager::chunkDir = chunkDir;
    return true;
}

std::string ChunkManager::getFileName(int x, int z) {
    std::string filename = "c." + std::to_string(x) + "." + std::to_string(z) + ".chunk";
    return filename;
}

bool ChunkManager::write(int x, int z, std::vector<idType>& chunkData){
    int region_x = x/chunks_per_region_side;
    int region_z = z/chunks_per_region_side;

    int chunk_x = x % chunks_per_region_side;
    int chunk_z = z % chunks_per_region_side;

    fs::path filePath;
    for(auto& entry: fs::directory_iterator(chunkDir)){
        if (entry.path().filename().string() == getFileName(region_x, region_z)){
            filePath = entry.path();
            break;
        }
    }

    if (filePath.empty()) {
        std::cerr << "File " << filePath.string() << " is empty ... cannot render chunk " <<
        chunk_x << ", " << chunk_z << std::endl;
        return false;
    }

    std::ofstream file(filePath, std::ios::binary | std::ios::out | std::ios::in);
    if (!file){
        std::cerr << "Failed to open file " << filePath.string() << std::endl;
        return false;
    }

    return true;
}