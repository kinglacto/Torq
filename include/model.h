#pragma once

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

