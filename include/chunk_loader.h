#pragma once

#include "chunk_utility.h"
#include "torq_utility.h"
#include "worldgen.hpp"
#include "thread_safe_queue.h"
#include "thread_pool.h"
#include <string>
#include <memory>
#include <unordered_map>
#include <filesystem>
#include <vector>
#include <unordered_set>
#include <mutex>
#include <thread>
#include <atomic>

#define REGION_FILE_EXTENSION "region"
#define REGION_FILE_PREAMBLE "r"
#define REGION_FILE_SEPARATOR "."
#define REGION_FILE_SEPARATOR_CHAR '.'

enum IOTaskCode{
    DEFAULT,
    WRITE_CHUNK,
    GET_CHUNK,
    WRITE_REGION,
    GENERATE_WRITE_REGION,
};

struct FileResult{
    std::shared_ptr<ChunkData> chunkData;
    int x, z;
    IOTaskCode taskCode;
    ChunkErrorCode err;
};

struct FileRequest{
    IOTaskCode taskCode;
    int x, z;
    std::shared_ptr<ChunkData> chunkData{nullptr};
    std::shared_ptr<RegionData> regionData{nullptr};
};

namespace fs = std::filesystem;

class ChunkLoader{
    static constexpr size_t header_size = CHUNKS_PER_REGION_SIDE * CHUNKS_PER_REGION_SIDE * 
    sizeof(HeaderEntry);

    ThreadPool threadPool;

    fs::path chunkDir;

    ChunkErrorCode createRegionFile(int x, int z);

    std::mutex generated_regions_mutex;
    std::unordered_set<std::pair<int, int>, PairHash> GeneratedRegions;

    std::mutex file_map_mutex_lock;
    std::unordered_map<std::pair<int, int>, std::shared_ptr<std::mutex>, PairHash> file_mutexes;

    std::shared_ptr<std::mutex> get_file_mutex(std::pair<int, int>& region_coords);

public:
    bool isRegionGenerated(const std::pair<int, int>& region_coords);
    void markRegionGenerated(const std::pair<int, int>& region_coords);

    ThreadSafeQueue<std::shared_ptr<FileResult>> FileResultQueue;
    ThreadSafeQueue<std::shared_ptr<FileRequest>> FileRequestQueue;

    explicit ChunkLoader(fs::path chunkDir, size_t num_threads);
    ChunkErrorCode cacheRegionFile(int region_x, int region_z);

    void writeChunkSync(std::shared_ptr<ChunkData> chunkData, IOTaskCode taskCode);
    void getChunkSync(int chunk_x, int chunk_z, IOTaskCode taskCode);
    void writeRegionSync(std::shared_ptr<RegionData> regionData, IOTaskCode);
    void unlocked_writeRegionSync(std::shared_ptr<RegionData> regionData, IOTaskCode taskCode);

    ChunkErrorCode setChunkDir(const fs::path& chunkDir);
    fs::path getRegionFilePath(int region_x, int region_z) const;

    void queueRequest(std::shared_ptr<FileRequest> request);
};