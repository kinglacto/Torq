#include <chunk_renderer.h>

void ChunkRenderer::deleteActiveChunk(const std::pair<int, int>& chunk_coords){
    auto activePtr = activeChunks.find(chunk_coords);
    if (activePtr != activeChunks.end()) {
        if (activePtr->second->chunkEverUpdated){
            auto f = std::make_shared<FileRequest>(WRITE_CHUNK);
            f->chunkData = std::move(activePtr->second->chunk);
            chunkLoader->queueRequest(std::move(f));
        }

        activeChunks.erase(activePtr);
    }
}

void ChunkRenderer::deleteActiveChunk(std::shared_ptr<ActiveChunk> activeChunk){
    if (activeChunk->chunkEverUpdated){
        auto f = std::make_shared<FileRequest>(WRITE_CHUNK);
        f->chunkData = std::move(activeChunk->chunk);
        chunkLoader->queueRequest(std::move(f));
    }
}

void ChunkRenderer::deleteChunkMesh(const std::pair<int, int>& chunk_coords){
    auto staticPtr = chunkMeshes.find(chunk_coords);
    if (staticPtr != chunkMeshes.end()) {
        staticPtr->second->deleteMesh();
        chunkMeshes.erase(staticPtr);
        deleteActiveChunk(chunk_coords);
    }
}

void ChunkRenderer::deleteChunkMesh(ChunkMesh* chunkMesh){
    std::pair<int, int> chunk_coords{chunkMesh->chunk_x, chunkMesh->chunk_z};
    chunkMesh->deleteMesh();
    deleteActiveChunk(chunk_coords);
}

void ChunkRenderer::makeChunkMesh(const std::pair<int, int>& chunk_coords,
std::shared_ptr<ChunkData> chunkData){
    auto staticPtr = chunkMeshes.find(chunk_coords);
    if (staticPtr == chunkMeshes.end()) {
        auto chunkMesh = std::make_shared<ChunkMesh>(chunk_coords.first, chunk_coords.second);
        ChunkMeshQueue.push({chunkMesh, std::move(chunkData)});
        chunkMeshes.emplace(chunk_coords, std::move(chunkMesh)); 
    }
}

void ChunkRenderer::makeChunkMeshRequest(const std::pair<int, int>& chunk_coords){
    auto staticPtr = chunkMeshes.find(chunk_coords);
    if (staticPtr == chunkMeshes.end()) {
        auto f = std::make_shared<FileRequest>(GET_CHUNK, chunk_coords.first, chunk_coords.second);
        chunkLoader->queueRequest(std::move(f));
    }
}

void ChunkRenderer::makeChunkActive(const std::pair<int, int>& chunk_coords, 
    std::shared_ptr<ChunkData> chunkData){
    auto activePtr = activeChunks.find(chunk_coords);
    if (activePtr != activeChunks.end()){
        return;
    }

    auto activeChunk = std::make_shared<ActiveChunk>(std::move(chunkData));
    ActiveChunkRemeshQueue.push(activeChunk);
    activeChunks.emplace(chunk_coords, std::move(activeChunk));

    auto staticPtr = chunkMeshes.find(chunk_coords);
    if (staticPtr == chunkMeshes.end()){
        auto chunkMesh = std::make_shared<ChunkMesh>(chunk_coords.first, chunk_coords.second);
        chunkMeshes.emplace(chunk_coords, std::move(chunkMesh));
    }
}

void ChunkRenderer::makeChunkActiveRequest(const std::pair<int, int>& chunk_coords){
    auto activePtr = activeChunks.find(chunk_coords);
    if (activePtr != activeChunks.end()){
        return;
    }
    auto f = std::make_shared<FileRequest>(GET_CHUNK, chunk_coords.first, chunk_coords.second);
    chunkLoader->queueRequest(std::move(f));
}

void ChunkRenderer::updateMeshes(){
    int chunk_x, chunk_z;

    {
        std::lock_guard<std::mutex> lock(world_pos_mutex);
        getChunkCoordsFromWorldCoords(static_cast<int>(worldPos.x), 
    static_cast<int>(worldPos.z), &chunk_x, &chunk_z);
    }

    std::shared_ptr<FileResult> f;
    while (chunkLoader->FileResultQueue.try_pop(f)) {
        if (f->taskCode == GET_CHUNK){
            std::pair<int, int> p = {f->x, f->z};
            pendingChunkRequests.erase(p);

            if (f->err == NO_ERROR && f->chunkData != nullptr){
                const int chebyshev = std::max(std::abs(f->x  - chunk_x),
                    std::abs(f->z - chunk_z));

                if (chebyshev <= activeRadius){
                    makeChunkActive(p, std::move(f->chunkData));
                }

                else{
                    makeChunkMesh(p, std::move(f->chunkData));
                }
            }

            else{
                // ToDo: Handle Read Errors
                std::cout << "read error detected but handling hasn't fully been implemented yet: "
                << f->err << " for " << f->x << " " << f->z << std::endl;
            }
        }

        else{
            // ToDo: Handle Write Errors
            std::cout << "write error detected but handling hasn't fully been implemented yet: "
                << f->err << " for " << f->x << " " << f->z << std::endl;
        }

        f.reset();
    }

    for(auto & activeChunk : activeChunks){
        if (activeChunk.second->remeshNeeded){
            ActiveChunkRemeshQueue.push(activeChunk.second);
        }
    }

    while(!ChunkMeshQueue.empty()){
        auto& [fst, snd] = ChunkMeshQueue.front();
        generateStaticMesh(fst, snd);
        ChunkMeshQueue.pop();
    }

    while(!ActiveChunkRemeshQueue.empty()){
        generateActiveMesh(ActiveChunkRemeshQueue.front());
        ActiveChunkRemeshQueue.pop();
    }
}

void ChunkRenderer::cleanup() {
    for (auto& mesh: chunkMeshes) {
        mesh.second->deleteMesh();
    }
}

ChunkErrorCode ChunkRenderer::generateStaticMesh(
    std::shared_ptr<ChunkMesh>& mesh, std::shared_ptr<ChunkData>& it){
    std::vector<TextureVertex> vertices;
    ChunkErrorCode e = generateVertices(it.get(), vertices);
    if (e != NO_ERROR) return e;
    mesh->uploadMesh(vertices);
    return NO_ERROR;
}

ChunkErrorCode ChunkRenderer::generateActiveMesh(std::shared_ptr<ActiveChunk>& activeChunk){
    ChunkErrorCode e = generateVertices(activeChunk->chunk.get(), activeChunk->vertices);
    if (e != NO_ERROR) return e;
    auto staticPtr = chunkMeshes.find(activeChunk->chunk_coords);
    staticPtr->second->uploadMesh(activeChunk->vertices);
    activeChunk->remeshNeeded = false;
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
                            glm::vec4 a = 
                            texture->uvMap[blockTexMap[chunkData->blocks[y][x][z].id].texId[i]];
                            float bu = a.x;
                            float bv = a.y;

                            float cu = a.z;
                            float cv = a.w;

                            float width = cu - bu;
                            float height = cv - bv;

                            for(int j = 0; j < 6; j++){
                                TextureVertex vert;
                                int v_index = j * 8;
                                vert.pos.x = blockVertices[i][v_index + 0] + static_cast<float>(x);
                                vert.pos.y = blockVertices[i][v_index + 1] + static_cast<float>(y);
                                vert.pos.z = blockVertices[i][v_index + 2] + static_cast<float>(z);

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
        if (chunkMesh->vertexCount == 0) continue;
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(coords.first * BLOCK_X_SIZE, 0.0f,
        coords.second * BLOCK_Z_SIZE));
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

void ChunkRenderer::update(){
    int chunk_x, chunk_z;

    {
        std::lock_guard<std::mutex> lock(world_pos_mutex);
        getChunkCoordsFromWorldCoords(static_cast<int>(worldPos.x), 
    static_cast<int>(worldPos.z), &chunk_x, &chunk_z);
    }

    for (auto it = activeChunks.begin(); it != activeChunks.end();) {
        int dx = std::abs(it->first.first  - chunk_x);
        int dz = std::abs(it->first.second - chunk_z);

        if (std::max(dx, dz) > activeRadius) {
            deleteActiveChunk(it->second);
            it = activeChunks.erase(it);
        }
        else {
            ++it;
        }
    }

    for (auto it = chunkMeshes.begin(); it != chunkMeshes.end();) {
        int dx = std::abs(it->first.first  - chunk_x);
        int dz = std::abs(it->first.second - chunk_z);

        if (std::max(dx, dz) > renderDistance) {
            deleteChunkMesh(it->second.get());
            it = chunkMeshes.erase(it);
        }
        else {
            ++it;
        }
    }

    for(int i = chunk_x - activeRadius; i <= chunk_x + activeRadius; i++){
        for(int j = chunk_z - activeRadius; j <= chunk_z + activeRadius; j++){
            std::pair<int, int> chunk_coords = {i, j};
            if (!pendingChunkRequests.contains(chunk_coords))
            {
                pendingChunkRequests.insert(chunk_coords);
                makeChunkActiveRequest(chunk_coords);
            }
        }
    }

    for(int i = chunk_x - renderDistance; i <= chunk_x + renderDistance; i++){
        for(int j = chunk_z - renderDistance; j <= chunk_z + renderDistance; j++){
            std::pair<int, int> chunk_coords = {i, j};
            if (!pendingChunkRequests.contains(chunk_coords))
            {
                pendingChunkRequests.insert(chunk_coords);
                makeChunkMeshRequest(chunk_coords);
            }
        }
    }

    updateMeshes();
}


void ChunkRenderer::setWorldPos(glm::vec3 worldPos){
    std::lock_guard<std::mutex> lock(world_pos_mutex);
    ChunkRenderer::worldPos = worldPos;
}