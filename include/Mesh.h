#pragma once

#include "Shader.h"
#include <glad/glad.h>
#include <string>
#include <vector>

struct Texture {
	enum Type {
		diffuse,
		specular
	};

	unsigned int id{ 0 };
	Type type;
};


class Mesh {
public:
	Mesh(const std::vector<float>& vertices,
		 const std::vector<unsigned int>& attribSizes,
		 const std::vector<Texture>& textures = {},
		 const std::vector<unsigned int>& indices = {});
	
	void draw(const Shader& shader) const;
	void drawInstanced(const Shader& shader, int count) const;

	unsigned int getVAO() const;
	int getAttribCount() const;

private:
	unsigned int VAO{ 0 }, VBO{ 0 }, EBO{ 0 };
	bool usingEBO{ false };
	int indicesCount;
	int verticesCount;
	int attribCount;
	std::vector<Texture> textures;

	void activateTextures(const Shader& shader) const;
};
