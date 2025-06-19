#include <active_chunk.h>

ActiveChunk::ActiveChunk(std::unique_ptr<ChunkData> chunk): 
chunk{std::move(chunk)}, remeshNeeded{false}, chunkEverUpdated{false}, vertexCount{0} {
    chunk_coords.first = chunk->x;
    chunk_coords.second = chunk->z;
}

ChunkErrorCode ActiveChunk::setBlock(int x, int y, int z, blockData data){
    if (!chunk) return CHUNK_CORRUPTED;
    if (x < 0 || x >= BLOCK_X_SIZE ||
        y < 0 || y >= BLOCK_Y_SIZE ||
        z < 0 || z >= BLOCK_Z_SIZE)
        return CHUNK_NOT_FOUND;

    chunk->blocks[y][x][z] = data;
    remeshNeeded       = true;
    chunkEverUpdated   = true;
    return NO_ERROR;
}