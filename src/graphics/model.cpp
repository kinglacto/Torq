#include "model.h"

PrimitiveModel::PrimitiveModel() = default;

void PrimitiveModel::renderAll() {
	for (const PrimitiveMesh& mesh: meshes) {
		mesh.render();
	}
}

void PrimitiveModel::cleanup() {
	for (auto & mesh : meshes) {
		mesh.cleanup();
	}
}



TextureModel::TextureModel() = default;

void TextureModel::renderAll() {
	for (const TextureMesh& mesh: meshes) {
		mesh.render();
	}
}

void TextureModel::cleanup() {
	for (auto & mesh : meshes) {
		mesh.cleanup();
	}
}