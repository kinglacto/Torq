#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include "mesh.h"

class PrimitiveModel {
public:
	std::vector<PrimitiveMesh> meshes;
	PrimitiveModel();
	void renderAll();
	void cleanup();
};

class TextureModel {
public:
	std::vector<TextureMesh> meshes;
	TextureModel();
	void renderAll();
	void cleanup();
};

#endif //MODEL_H
