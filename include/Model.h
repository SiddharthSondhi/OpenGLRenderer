#pragma once
#include "Shader.h"
#include "Mesh.h"
#include "Material.h"
#include "Texture.h"

#include <assimp/scene.h>

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

struct ModelLoadOptions {
	bool flipUVs{ false };
	bool calcTangentSpace{ false };
	bool genSmoothNormals{ true };
};

class Model {
public:
	Model(std::string path, const ModelLoadOptions& options = {});
	Model(const Mesh& mesh, const Material& material);

	const std::vector<Mesh>& getMeshes() const;
	const std::vector<std::unique_ptr<Material>>& getMaterials() const;

private:
	std::string directory;
	std::vector<Mesh> meshes;
	std::vector<std::unique_ptr<Material>> materials;
	std::unordered_map<std::string, Texture> loadedTextures;

	void processNode(aiNode* node, const aiScene* scene, const ModelLoadOptions& options);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene, const ModelLoadOptions& options);
	void processMaterials(const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, const aiScene* scene);
};