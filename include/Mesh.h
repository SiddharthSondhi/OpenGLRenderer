#pragma once

#include <vector>

class Mesh {
public:
	Mesh() = default;

	Mesh(const std::vector<float>& vertices,
		 const std::vector<unsigned int>& attribSizes,
		 size_t materialIndex = 0,
		 const std::vector<unsigned int>& indices = {});
	
	void drawGeometry() const;
	void drawInstancedGeometry(int count) const;

	unsigned int getVAO() const;
	int getAttribCount() const;
	size_t getMaterialIndex() const;

private:
	unsigned int VAO{ 0 }, VBO{ 0 }, EBO{ 0 };
	bool usingEBO{ false };
	int indicesCount{ 0 };
	int verticesCount{ 0 };
	int attribCount{ 0 };

	// index of corresponding material in the model that contains this mesh
	size_t materialIndex{ 0 };
};
