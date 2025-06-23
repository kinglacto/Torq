#include <chunk_renderer.h>

ChunkErrorCode ChunkRenderer::deleteActiveChunk(const std::pair<int, int>& chunk_coords){
    auto activePtr = activeChunks.find(chunk_coords);
    if (activePtr != activeChunks.end()) {
        if (activePtr->second->chunkEverUpdated){
            chunkLoader->writeChunk(activePtr->second->chunk.get());
        }

        activeChunks.erase(activePtr);
        return NO_ERROR;
    }

    return CHUNK_NOT_FOUND;
}

ChunkErrorCode ChunkRenderer::deleteChunkMesh(const std::pair<int, int>& chunk_coords){
    auto staticPtr = chunkMeshes.find(chunk_coords);
    if (staticPtr != chunkMeshes.end()) {
        staticPtr->second->deleteMesh();
        chunkMeshes.erase(staticPtr);
        deleteActiveChunk(chunk_coords);
    }

    return CHUNK_NOT_FOUND;
}

ChunkErrorCode ChunkRenderer::makeChunkMesh(const std::pair<int, int>& chunk_coords){
    auto staticPtr = chunkMeshes.find(chunk_coords);
    if (staticPtr == chunkMeshes.end()) {
        ChunkErrorCode e;
        auto chunkData = std::move(chunkLoader->getChunk(chunk_coords.first, chunk_coords.second, &e));
        if (e == NO_ERROR){
            auto chunkMesh = std::make_unique<ChunkMesh>(chunk_coords.first, chunk_coords.second);
            ChunkMeshQueue.push({chunkMesh.get(), std::move(chunkData)});
            chunkMeshes.emplace(chunk_coords, std::move(chunkMesh));
        }   

        else{
            return e;
        }
    }

    return NO_ERROR;
}

ChunkErrorCode ChunkRenderer::makeChunkActive(const std::pair<int, int>& chunk_coords){
    auto activePtr = activeChunks.find(chunk_coords);
    if (activePtr != activeChunks.end()){
        return NO_ERROR;
    }

    else{
        ChunkErrorCode e;
        auto chunkData = std::move(chunkLoader->getChunk(chunk_coords.first, chunk_coords.second, &e));
        if (e == NO_ERROR){
            auto activeChunk = std::make_unique<ActiveChunk>(std::move(chunkData));
            ActiveChunkRemeshQueue.push(activeChunk.get());
            activeChunks.emplace(chunk_coords, std::move(activeChunk));

            auto staticPtr = chunkMeshes.find(chunk_coords);
            if (staticPtr == chunkMeshes.end()){
                auto chunkMesh = std::make_unique<ChunkMesh>(chunk_coords.first, chunk_coords.second);
                chunkMeshes.emplace(chunk_coords, std::move(chunkMesh));
            }
        }   

        else{
            return e;
        }
    }
}

void ChunkRenderer::updateMeshes(){
    for(auto it = activeChunks.begin(); it != activeChunks.end(); it++){
        if (it->second->remeshNeeded){
            ActiveChunkRemeshQueue.push(it->second.get());
        }
    }

    while(!ChunkMeshQueue.empty()){
        generateStaticMesh(ChunkMeshQueue.front());
        ChunkMeshQueue.pop();
    }

    while(!ActiveChunkRemeshQueue.empty()){
        generateActiveMesh(ActiveChunkRemeshQueue.front());
        ActiveChunkRemeshQueue.pop();
    }
}

ChunkErrorCode ChunkRenderer::generateStaticMesh(std::pair<ChunkMesh*, std::unique_ptr<ChunkData>>& it){
    std::vector<TextureVertex> vertices;
    generateVertices(it.second.get(), vertices);
    it.first->uploadMesh(vertices);
    return NO_ERROR;
}

ChunkErrorCode ChunkRenderer::generateActiveMesh(ActiveChunk* activeChunk){
    generateVertices(activeChunk->chunk.get(), activeChunk->vertices);
    auto staticPtr = chunkMeshes.find(activeChunk->chunk_coords);
    staticPtr->second->uploadMesh(activeChunk->vertices);
    return NO_ERROR;
}


inline bool isBlockSolid(ChunkData* chunkData, int x, int y, int z) {
    if (x < 0 || x >= BLOCK_X_SIZE || 
        y < 0 || y >= BLOCK_Y_SIZE || 
        z < 0 || z >= BLOCK_Z_SIZE) {
        return false; // Treat out-of-bounds as air for now
    }
    // Assuming ID 0 is air and any other ID is a solid block
    return chunkData->blocks[y][x][z].id != 0;
}

ChunkErrorCode ChunkRenderer::generateVertices(ChunkData* chunkData, 
    std::vector<TextureVertex>& vertices) {

    glm::vec4 a = texture->uvMap[texMap::block];
	float bu = a.x;
	float bv = a.y;

	float cu = a.z;
	float cv = a.w;

	float width = cu - bu;
	float height = cv - bv;
    
    vertices.clear();
    vertices.reserve(10000); 

    for(int y = 0; y < BLOCK_Y_SIZE; y++){
        for(int x = 0; x < BLOCK_X_SIZE; x++){
            for(int z = 0; z < BLOCK_Z_SIZE; z++){
                if (isBlockSolid(chunkData, x, y, z)){
                    for(int i = 0; i < 6; i++){
                        int new_x = x + neighbor_offsets[i][0];
                        int new_y = y + neighbor_offsets[i][1];
                        int new_z = z + neighbor_offsets[i][2];

                        if (!isBlockSolid(chunkData, new_x, new_y, new_z)){
                            for(int j = 0; j < 6; j++){
                                TextureVertex vert;
                                int v_index = j * 8;
                                vert.pos.x = blockVertices[i][v_index + 0] + x; 
                                vert.pos.y = blockVertices[i][v_index + 1] + y;
                                vert.pos.z = blockVertices[i][v_index + 2] + z;

                                vert.normal.x = blockVertices[i][v_index + 3];
                                vert.normal.y = blockVertices[i][v_index + 4];
                                vert.normal.z = blockVertices[i][v_index + 5];

                                vert.tex.x = blockVertices[i][v_index + 6] * width + bu;
                                vert.tex.y = blockVertices[i][v_index + 7] * height + bv;
                                
                                vertices.push_back(vert);
                            }
                        }
                    }
                }
            }
        }
    }
    
    return NO_ERROR;
}

void ChunkRenderer::render(Shader* shader){
    for(const auto& [coords, chunkMesh] : chunkMeshes) {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(coords.first * BLOCK_X_SIZE, 0.0f, 
            coords.second * BLOCK_Z_SIZE)) * glm::scale(glm::mat4(1.0f), block_scaling_factor);

        shader->setMat4("model", model);
        chunkMesh->render(shader);
    }
}

void ChunkRenderer::getRegionCoordsFromChunkCoords(int chunk_x, int chunk_z, int* region_x, int* region_z){
    *region_x = static_cast<int>(std::floor(static_cast<float>(chunk_x) / CHUNKS_PER_REGION_SIDE));
    *region_z = static_cast<int>(std::floor(static_cast<float>(chunk_z) / CHUNKS_PER_REGION_SIDE));
}

void ChunkRenderer::getChunkCoordsFromWorldCoords(int world_x, int world_z, int* chunk_x, int* chunk_z){
    *chunk_x = static_cast<int>(std::floor(static_cast<float>(world_x) / BLOCK_X_SIZE));
    *chunk_z = static_cast<int>(std::floor(static_cast<float>(world_z) / BLOCK_Z_SIZE));
}   

void ChunkRenderer::getRegionCoordsFromWorldCoords(int world_x, int world_z, int* region_x, int* region_z){
    int chunk_x, chunk_z;
    getChunkCoordsFromWorldCoords(world_x, world_z, &chunk_x, &chunk_z);
    getRegionCoordsFromChunkCoords(chunk_x, chunk_z, region_x, region_z);
}

void ChunkRenderer::update(Shader* shader){
    getChunkCoordsFromWorldCoords(worldPos.x, worldPos.z, &chunk_x, &chunk_z);

    makeChunkMesh({chunk_x, chunk_z});
    updateMeshes();
    render(shader);
}