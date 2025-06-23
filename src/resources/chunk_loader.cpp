#include <chunk_utility.h>
#include <chunk_loader.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include <cmath>
#include <zlib.h>
#include <array>

namespace fs = std::filesystem;

ChunkLoader::ChunkLoader(const std::string& chunkDir){
    setChunkDir(chunkDir);
}

ChunkErrorCode ChunkLoader::createRegionFile(int region_x, int region_z){
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

ChunkErrorCode ChunkLoader::cacheRegionFile(int region_x, int region_z){
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

std::unique_ptr<ChunkData> ChunkLoader::getChunk(int chunk_x, int chunk_z, ChunkErrorCode* error){
    int region_x = static_cast<int>(std::floor(static_cast<float>(chunk_x) / CHUNKS_PER_REGION_SIDE));
    int region_z = static_cast<int>(std::floor(static_cast<float>(chunk_z) / CHUNKS_PER_REGION_SIDE));

    int chunk_x_index = MathMod(chunk_x, CHUNKS_PER_REGION_SIDE);
    int chunk_z_index = MathMod(chunk_z, CHUNKS_PER_REGION_SIDE);

    int header_index = chunk_x_index * CHUNKS_PER_REGION_SIDE + chunk_z_index;

    auto region_coords = std::make_pair(region_x, region_z);
    if (ExistingFiles.find(region_coords) == ExistingFiles.end()){
        *error = CHUNK_EMPTY;
        return nullptr;
    }

    std::string filePath = getRegionFileName(region_x, region_z);

    std::ifstream file(filePath, std::ios::binary);
    if (!file){
        std::cerr << "Failed to open file " << filePath << std::endl;
        *error = FILE_ERROR;
        return nullptr;
    }

    std::vector<HeaderEntry> headerBuffer(CHUNKS_PER_REGION_SIDE * CHUNKS_PER_REGION_SIDE);
    if (!file.read(reinterpret_cast<char*>(headerBuffer.data()), header_size)){
        std::cerr << "Failed to read the header in: " << filePath << std::endl;
        *error = HEADER_CORRUPTED;
        return nullptr;
    }

    auto& entry = headerBuffer[header_index];
    size_t offset = static_cast<size_t>(entry.offset);
    size_t length = static_cast<size_t>(entry.length);

    if (offset == 0 || length == 0) {
        std::cout << "Chunk (" << chunk_x_index << ", " << chunk_z_index << ") not found in region file: ("
        << region_x << ", " << region_z << ") . Please write it." << std::endl;
        *error = CHUNK_EMPTY;
        return nullptr;
    }

    if (offset < header_size) {
        std::cerr << "Chunk in " << filePath << " has corrupted offset pointing into header.\n";
        *error = CHUNK_CORRUPTED;
        return nullptr;
    }


    std::vector<uint8_t> compressedChunkBuffer(length);
    file.seekg(static_cast<std::streamoff>(offset), std::ios::beg);

    if (!file.read(reinterpret_cast<char*>(compressedChunkBuffer.data()), length)){
        std::cerr << "Failed to read the chunk data in: " << filePath << std::endl;
        *error = FILE_ERROR;
        return nullptr;
    }

    auto chunk = std::make_unique<ChunkData>();
    uLongf destLen = sizeof(chunk->blocks);
    int zret = uncompress(
        reinterpret_cast<Bytef*>(chunk->blocks),
        &destLen,
        compressedChunkBuffer.data(),
        static_cast<uLong>(length)
    );
    if (zret != Z_OK || destLen != sizeof(chunk->blocks)) {
        std::cerr << "Decompression failed\n";
        *error = COMPRESSION_ERROR;
        return nullptr;
    }

    chunk->x = chunk_x; 
    chunk->z = chunk_z;
    *error = NO_ERROR;
    return chunk;
}

ChunkErrorCode ChunkLoader::writeChunk(ChunkData* chunkData){
    int chunk_x = chunkData->x;
    int chunk_z = chunkData->z;

    int region_x = static_cast<int>(std::floor(static_cast<float>(chunk_x) / CHUNKS_PER_REGION_SIDE));
    int region_z = static_cast<int>(std::floor(static_cast<float>(chunk_z) / CHUNKS_PER_REGION_SIDE));

    int chunk_x_index = MathMod(chunk_x, CHUNKS_PER_REGION_SIDE);
    int chunk_z_index = MathMod(chunk_z, CHUNKS_PER_REGION_SIDE);

    int header_index = chunk_x_index * CHUNKS_PER_REGION_SIDE + chunk_z_index;

    auto region_coords = std::make_pair(region_x, region_z);
    if (ExistingFiles.find(region_coords) == ExistingFiles.end()){
        ChunkErrorCode e = createRegionFile(region_x, region_z);
        if (e != NO_ERROR){
            return e;
        }
        ExistingFiles.insert(region_coords);
    }

    std::string filePath = getRegionFileName(region_x, region_z);

    uLongf compressedSize = compressBound(sizeof(chunkData->blocks));
    std::vector<Bytef> compressedBuffer(compressedSize);
    
    int zret = compress(
        compressedBuffer.data(), 
        &compressedSize, 
        reinterpret_cast<const Bytef*>(chunkData->blocks), 
        sizeof(chunkData->blocks)
    );

    if (zret != Z_OK) {
        std::cerr << "Compression failed for chunk (" << chunk_x << ", " << chunk_z << ")\n";
        return COMPRESSION_ERROR;
    }

    compressedBuffer.resize(compressedSize);

    std::fstream file(filePath, std::ios::in | std::ios::out | std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open region file for writing: " << filePath << "\n";
        return FILE_ERROR;
    }

    std::vector<HeaderEntry> header(CHUNKS_PER_REGION_SIDE * CHUNKS_PER_REGION_SIDE);
    file.seekg(0, std::ios::beg);
    if (!file.read(reinterpret_cast<char*>(header.data()), header_size)) {
        std::cerr << "Failed to read header from " << filePath << " for writing.\n";
        return HEADER_CORRUPTED;
    }

    file.seekp(0, std::ios::end);
    auto newOffset = static_cast<chunk_header_offset_type>(file.tellp());
    auto newLength = static_cast<chunk_header_length_type>(compressedBuffer.size());

    if (newOffset < header_size) {
        newOffset = header_size;
        file.seekp(newOffset, std::ios::beg);
    }

    file.write(reinterpret_cast<const char*>(compressedBuffer.data()), newLength);
    if (!file) {
        std::cerr << "Failed to write chunk data to " << filePath << "\n";
        return FILE_ERROR;
    }

    header[header_index].offset = newOffset;
    header[header_index].length = newLength;

    file.seekp(0, std::ios::beg);
    file.write(reinterpret_cast<const char*>(header.data()), header_size);
    if (!file) {
        std::cerr << "Failed to write updated header to " << filePath << "\n";
        return HEADER_CORRUPTED;
    }

    return NO_ERROR;
    
}

ChunkErrorCode ChunkLoader::writeRegion(RegionData* regionData){
    int region_x = regionData->x;
    int region_z = regionData->z;

    auto region_coords = std::make_pair(region_x, region_z);

    if (ExistingFiles.find(region_coords) == ExistingFiles.end()){
        ChunkErrorCode e = createRegionFile(region_x, region_z);
        if (e != NO_ERROR){
            return e;
        }
        ExistingFiles.insert(region_coords);
    }

    std::vector<HeaderEntry> header(CHUNKS_PER_REGION_SIDE * CHUNKS_PER_REGION_SIDE);

    std::vector<Bytef> final_payload;
    const size_t total_uncompressed_size = sizeof(ChunkData::blocks) * CHUNKS_PER_REGION_SIDE * 
    CHUNKS_PER_REGION_SIDE;

    final_payload.reserve(total_uncompressed_size / 2);

    std::size_t offset = header_size;
    uLongf compress_bound_size = compressBound(sizeof(ChunkData::blocks));
    std::vector<Bytef> compressedBuffer(compress_bound_size);
    for(int i = 0; i < CHUNKS_PER_REGION_SIDE; i++){
        for(int j = 0; j < CHUNKS_PER_REGION_SIDE; j++){
            uLongf compressedSize = compressBound(sizeof(regionData->chunks[i][j].blocks));
    
            int zret = compress(    
                compressedBuffer.data(), 
                &compressedSize, 
                reinterpret_cast<const Bytef*>(regionData->chunks[i][j].blocks),
                sizeof(regionData->chunks[i][j].blocks)
            );

            if (zret != Z_OK) {
                std::cerr << "Compression failed for chunk (" << i << ", " << j << ")\n";
                return COMPRESSION_ERROR;
            }

            compressedBuffer.resize(compressedSize);

            auto newOffset = static_cast<chunk_header_offset_type>(offset);
            auto newLength = static_cast<chunk_header_length_type>(compressedBuffer.size());
            offset += compressedBuffer.size();

            header[i * CHUNKS_PER_REGION_SIDE + j].offset = newOffset;
            header[i * CHUNKS_PER_REGION_SIDE + j].length = newLength;

            final_payload.insert(final_payload.end(), compressedBuffer.begin(), 
            compressedBuffer.begin() + compressedSize);
        }
    }

    std::string filePath = getRegionFileName(region_x, region_z);
    std::fstream file(filePath, std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc);
    if (!file) {
        std::cerr << "Failed to open region file for writing: " << filePath << "\n";
        return FILE_ERROR;
    }

    file.write(reinterpret_cast<const char*>(final_payload.data()), final_payload.size());
    if (!file) {
        std::cerr << "Failed to write chunk data to " << filePath << "\n";
        return FILE_ERROR;
    }

    file.seekg(0, std::ios::beg);
    file.write(reinterpret_cast<const char*>(header.data()), header_size);
    if (!file) {
        std::cerr << "Failed to write updated header to " << filePath << "\n";
        return HEADER_CORRUPTED;
    }

    return NO_ERROR;

}

ChunkErrorCode ChunkLoader::setChunkDir(const std::string& chunkDir){
    if (fs::exists(chunkDir)){
        ChunkLoader::chunkDir = chunkDir;
    }

    else{
        std::error_code ec;
        if (fs::create_directories(chunkDir, ec)) {
            ChunkLoader::chunkDir = chunkDir;
        } else {
            std::cerr << "Failed to create directory: " << chunkDir << "\n"
                      << "Reason: " << ec.message() << std::endl;
            return DIRECTORY_CREATION_FAILED;
        }
    }

    try {
        for (const auto& entry : fs::directory_iterator(chunkDir)) {
            if (entry.is_regular_file()) {
                std::string name = entry.path().filename().string();
                // Expect “c.<rx>.<rz>.region”
                std::array<std::string,4> parts;
                {
                    std::istringstream iss(name);
                    for (int i = 0; i < 4; ++i) {
                        if (!std::getline(iss, parts[i], '.'))
                            break;
                    }
                }
                if (parts[0] == "c" && parts[3] == "region") {
                    try {
                        int rx = std::stoi(parts[1]);
                        int rz = std::stoi(parts[2]);
                        ExistingFiles.emplace(rx, rz);
                    } catch (...) {
                        std::cerr << "Bad region file name: " << name << "\n";
                    }
                }
            }
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << '\n';
        return FILE_ERROR;
    }

    return NO_ERROR;
}

std::string ChunkLoader::getRegionFileName(int region_x, int region_z){
    if (chunkDir.empty()) return "";
    return chunkDir + "/c." + std::to_string(region_x) + "." + std::to_string(region_z) + ".region";
}
