#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include "mesh.h"

class PrimitiveModel {
protected:
	std::vector<PrimitiveMesh> meshes;
	PrimitiveModel();
	void renderAll();
public:
	void cleanup();
};

class TextureModel {
protected:
	std::vector<TextureMesh> meshes;
	TextureModel();
	void renderAll();
public:
	void cleanup();
};

#endif //MODEL_H
