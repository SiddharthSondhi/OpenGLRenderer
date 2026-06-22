#pragma once
#include "Shader.h"
#include "Mesh.h"

#include <assimp/scene.h>

#include <string>
#include <vector>
#include <unordered_map>

class Model {
public:
	Model(std::string path, bool flipUVs);
	void draw(Shader& shader);

private:
	std::vector<Mesh> meshes;
	std::string directory;
	std::unordered_map<std::string, Texture> loadedTextures;

	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type);
};