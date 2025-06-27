#include <chunk_utility.h>
#include <chunk_loader.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include <cmath>
#include <zlib.h>
#include <array>

std::shared_ptr<std::mutex> ChunkLoader::get_file_mutex(std::pair<int, int>& region_coords) {
    std::lock_guard<std::mutex> lock(file_map_mutex_lock);
    auto& mtx_ptr = file_mutexes[region_coords];
    if (!mtx_ptr) {
        mtx_ptr = std::make_shared<std::mutex>();
    }
    return mtx_ptr;
}

bool ChunkLoader::isRegionGenerated(const std::pair<int, int>& region_coords) {
    std::lock_guard<std::mutex> lock(generated_regions_mutex);
    return GeneratedRegions.find(region_coords) != GeneratedRegions.end();
}

void ChunkLoader::markRegionGenerated(const std::pair<int, int>& region_coords) {
    std::lock_guard<std::mutex> lock(generated_regions_mutex);
    GeneratedRegions.insert(region_coords);
}

ChunkLoader::ChunkLoader(const fs::path chunkDir, size_t num_threads): threadPool(num_threads){
    setChunkDir(chunkDir);
}

ChunkErrorCode ChunkLoader::createRegionFile(int region_x, int region_z){
    fs::path filePath = getRegionFilePath(region_x, region_z);

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
    // NOTE: For now, we rely on Mr.Torvalds' page caching by warming up the required file
    // This obviously exists in brevity; isn't meant to be used as it is redundant
    // To be replaced by a robust custom file caching system

    fs::path filePath = getRegionFilePath(region_x, region_z);
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

void ChunkLoader::getChunkSync(int chunk_x, int chunk_z, IOTaskCode taskCode){
    int region_x = static_cast<int>(std::floor(static_cast<float>(chunk_x) / CHUNKS_PER_REGION_SIDE));
    int region_z = static_cast<int>(std::floor(static_cast<float>(chunk_z) / CHUNKS_PER_REGION_SIDE));

    int chunk_x_index = MathMod(chunk_x, CHUNKS_PER_REGION_SIDE);
    int chunk_z_index = MathMod(chunk_z, CHUNKS_PER_REGION_SIDE);

    int header_index = chunk_x_index * CHUNKS_PER_REGION_SIDE + chunk_z_index;

    auto region_coords = std::make_pair(region_x, region_z);

    auto file_mtx = get_file_mutex(region_coords);
    std::lock_guard<std::mutex> lock(*file_mtx);

    {
        std::lock_guard<std::mutex> regions_lock(generated_regions_mutex);
        if (!GeneratedRegions.contains(region_coords)) {
            auto regionData = std::make_shared<RegionData>(region_x, region_z);
            auto regionHM = std::make_unique<RHeightMap>(region_x, region_z);
            WorldGen::generateRegion(regionHM.get(), regionData.get());
            unlocked_writeRegionSync(std::move(regionData), GENERATE_WRITE_REGION);
            GeneratedRegions.insert(region_coords);
        }
    }

    auto f = std::make_shared<FileResult>();
    f->taskCode = taskCode;
    f->x = chunk_x;
    f->z = chunk_z;

    fs::path filePath = getRegionFilePath(region_x, region_z);
    if (!fs::exists(filePath)) {
        f->err = REGION_NOT_FOUND;
        FileResultQueue.push(std::move(f));
        return;
    }

    std::ifstream file(filePath, std::ios::binary);

    if (!file){
        f->err = FILE_ERROR;
        FileResultQueue.push(std::move(f));
        return;
    }

    file.seekg(0, std::ios::end);
    std::streampos fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    if (fileSize < static_cast<std::streampos>(header_size)) {
        f->err = HEADER_CORRUPTED;
        FileResultQueue.push(std::move(f));
        return;
    }

    std::vector<HeaderEntry> headerBuffer(CHUNKS_PER_REGION_SIDE * CHUNKS_PER_REGION_SIDE);
    if (!file.read(reinterpret_cast<char*>(headerBuffer.data()), header_size)){
        f->err = HEADER_CORRUPTED;
        FileResultQueue.push(std::move(f));
        return;
    }

    auto& entry = headerBuffer[header_index];
    auto offset = static_cast<size_t>(entry.offset);
    auto length = static_cast<size_t>(entry.length);

    if (offset == 0 || length == 0) {
        f->err = CHUNK_EMPTY;
        FileResultQueue.push(std::move(f));
        return;
    }

    if (offset < header_size) {
        f->err = CHUNK_CORRUPTED;
        FileResultQueue.push(std::move(f));
        return;
    }


    std::vector<uint8_t> compressedChunkBuffer(length);
    file.seekg(static_cast<std::streamoff>(offset), std::ios::beg);

    if (!file.read(reinterpret_cast<char*>(compressedChunkBuffer.data()), length)){
        f->err = FILE_ERROR;
        FileResultQueue.push(std::move(f));
        return;
    }

    if (file.gcount() != length) {
        f->err = CHUNK_CORRUPTED;
        FileResultQueue.push(std::move(f));
        return;
    }

    auto chunk = std::make_shared<ChunkData>();
    uLongf destLen = sizeof(chunk->blocks);
    int zret = uncompress(
        reinterpret_cast<Bytef*>(chunk->blocks),
        &destLen,
        compressedChunkBuffer.data(),
        static_cast<uLong>(length)
    );

    if (zret != Z_OK) {
        f->err = DECOMPRESSION_ERROR;
        FileResultQueue.push(std::move(f));
        return;
    }

    if (destLen != sizeof(chunk->blocks)) {
        f->err = CHUNK_CORRUPTED;
        FileResultQueue.push(std::move(f));
        return;
    }

    chunk->x = chunk_x; 
    chunk->z = chunk_z;
    f->err = NO_ERROR;
    f->chunkData = std::move(chunk);
    FileResultQueue.push(std::move(f));
}

void ChunkLoader::writeChunkSync(std::shared_ptr<ChunkData> chunkData, IOTaskCode taskCode){
    int chunk_x = chunkData->x;
    int chunk_z = chunkData->z;

    int region_x = static_cast<int>(std::floor(static_cast<float>(chunk_x) / CHUNKS_PER_REGION_SIDE));
    int region_z = static_cast<int>(std::floor(static_cast<float>(chunk_z) / CHUNKS_PER_REGION_SIDE));

    int chunk_x_index = MathMod(chunk_x, CHUNKS_PER_REGION_SIDE);
    int chunk_z_index = MathMod(chunk_z, CHUNKS_PER_REGION_SIDE);

    int header_index = chunk_x_index * CHUNKS_PER_REGION_SIDE + chunk_z_index;

    auto region_coords = std::make_pair(region_x, region_z);

    fs::path filePath = getRegionFilePath(region_x, region_z);

    uLongf compressedSize = compressBound(sizeof(chunkData->blocks));
    std::vector<Bytef> compressedBuffer(compressedSize);

    auto f = std::make_shared<FileResult>();
    f->taskCode = taskCode;
    f->x = chunk_x;
    f->z = chunk_z;
    
    int zret = compress(
        compressedBuffer.data(), 
        &compressedSize, 
        reinterpret_cast<const Bytef*>(chunkData->blocks), 
        sizeof(chunkData->blocks)
    );

    if (zret != Z_OK) {
        f->err = COMPRESSION_ERROR;
        FileResultQueue.push(std::move(f));
        return;
    }

    compressedBuffer.resize(compressedSize);

    auto file_mtx = get_file_mutex(region_coords); 
    std::lock_guard<std::mutex> lock(*file_mtx);

    std::fstream file(filePath, std::ios::in | std::ios::out | std::ios::binary);
    if (!file) {
        ChunkErrorCode createResult = createRegionFile(region_x, region_z);
        if (createResult != NO_ERROR) {
            f->err = createResult;
            FileResultQueue.push(std::move(f));
            return;
        }
        file.open(filePath, std::ios::in | std::ios::out | std::ios::binary);
        if (!file) {
            f->err = FILE_ERROR;
            FileResultQueue.push(std::move(f));
            return;
        }
    }

    std::vector<HeaderEntry> header(CHUNKS_PER_REGION_SIDE * CHUNKS_PER_REGION_SIDE);
    file.seekg(0, std::ios::beg);
    if (!file.read(reinterpret_cast<char*>(header.data()), header_size)) {
        f->err = HEADER_CORRUPTED;
        FileResultQueue.push(std::move(f));
        return;
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
        f->err = FILE_ERROR;
        FileResultQueue.push(std::move(f));
        return;
    }

    header[header_index].offset = newOffset;
    header[header_index].length = newLength;

    file.seekp(0, std::ios::beg);
    file.write(reinterpret_cast<const char*>(header.data()), header_size);
    if (!file) {
        f->err = FILE_ERROR;
        FileResultQueue.push(std::move(f));
        return;
    }
}

void ChunkLoader::writeRegionSync(std::shared_ptr<RegionData> regionData, IOTaskCode taskCode){
    int region_x = regionData->x;
    int region_z = regionData->z;

    auto region_coords = std::make_pair(region_x, region_z);
    auto file_mtx = get_file_mutex(region_coords);
    std::lock_guard<std::mutex> lock(*file_mtx);
    unlocked_writeRegionSync(std::move(regionData), taskCode);
}

void ChunkLoader::unlocked_writeRegionSync(std::shared_ptr<RegionData> regionData, IOTaskCode taskCode) {
    int region_x = regionData->x;
    int region_z = regionData->z;

    auto region_coords = std::make_pair(region_x, region_z);

    std::vector<HeaderEntry> header(CHUNKS_PER_REGION_SIDE * CHUNKS_PER_REGION_SIDE);

    std::vector<Bytef> final_payload;
    const size_t total_uncompressed_size = sizeof(ChunkData::blocks) * CHUNKS_PER_REGION_SIDE *
    CHUNKS_PER_REGION_SIDE;

    final_payload.reserve(total_uncompressed_size / 2);

    auto f = std::make_shared<FileResult>();
    f->taskCode = taskCode;
    f->x = regionData->x;
    f->z = regionData->z;

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
                f->err = COMPRESSION_ERROR;
                FileResultQueue.push(std::move(f));
                return;
            }

            auto newOffset = static_cast<chunk_header_offset_type>(offset);
            auto newLength = static_cast<chunk_header_length_type>(compressedSize);
            offset += compressedSize;

            header[i * CHUNKS_PER_REGION_SIDE + j].offset = newOffset;
            header[i * CHUNKS_PER_REGION_SIDE + j].length = newLength;

            final_payload.insert(final_payload.end(), compressedBuffer.begin(),
                     compressedBuffer.begin() + compressedSize);
        }
    }

    fs::path filePath = getRegionFilePath(region_x, region_z);
    std::fstream file(filePath, std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc);
    if (!file.is_open()) {
        std::ofstream createFile(filePath, std::ios::binary); // creates an empty file
        createFile.close();

        file.open(filePath, std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc);
    }

    file.seekp(0, std::ios::beg);
    file.write(reinterpret_cast<const char*>(header.data()), header_size);
    if (!file) {
        f->err = FILE_ERROR;
        FileResultQueue.push(std::move(f));
        return;
    }


    file.write(reinterpret_cast<const char*>(final_payload.data()), final_payload.size());
    if (!file) {
        f->err = FILE_ERROR;
        FileResultQueue.push(std::move(f));
        return;
    }
}
ChunkErrorCode ChunkLoader::setChunkDir(const fs::path& chunkDir){
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
                // Expecting “<preamble>.<rx>.<rz>.<extension>”
                std::array<std::string,4> parts;
                {
                    std::istringstream iss(name);
                    for (int i = 0; i < 4; ++i) {
                        if (!std::getline(iss, parts[i], REGION_FILE_SEPARATOR_CHAR))
                            break;
                    }
                }
                if (parts[0] == REGION_FILE_PREAMBLE && parts[3] == REGION_FILE_EXTENSION) {
                    try {
                        int rx = std::stoi(parts[1]);
                        int rz = std::stoi(parts[2]);
                        GeneratedRegions.emplace(rx, rz);
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

inline fs::path ChunkLoader::getRegionFilePath(const int region_x, const int region_z) const
{
    if (chunkDir.empty()) return fs::path{};
    std::string name;
    name.reserve(sizeof(REGION_FILE_PREAMBLE)-1
                   + sizeof(REGION_FILE_SEPARATOR)-1
                   + sizeof(REGION_FILE_SEPARATOR)-1
                   + sizeof(REGION_FILE_EXTENSION)-1
                   + sizeof(REGION_FILE_SEPARATOR)-1
                   + 4 /*max digits*/);
    name = std::string(REGION_FILE_PREAMBLE) + REGION_FILE_SEPARATOR
    + std::to_string(region_x) + REGION_FILE_SEPARATOR + std::to_string(region_z) +
        REGION_FILE_SEPARATOR + REGION_FILE_EXTENSION;
    fs::path fullPath = chunkDir / name;
    return fullPath;
}

void ChunkLoader::queueRequest(std::shared_ptr<FileRequest> f){
    threadPool.submit([this, f]() {
        switch (f->taskCode) {
        case GET_CHUNK: {
            getChunkSync(f->x, f->z, f->taskCode);
            break;
        }

        case WRITE_CHUNK: {
            writeChunkSync(std::move(f->chunkData), f->taskCode);
            break;
        }

        case WRITE_REGION: {
            writeRegionSync(std::move(f->regionData), f->taskCode);
            break;
        }
        default:
            break;
    }
    });
}