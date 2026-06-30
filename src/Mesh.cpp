#include "Mesh.h"
#include <numeric>
#include <iostream>

Mesh::Mesh(const std::vector<float>& vertices,
	const std::vector<unsigned int>& attribSizes,
	const std::vector<Texture>& textures,
	const std::vector<unsigned int>& indices)
	:
	textures{ textures },
	indicesCount{ static_cast<int>(indices.size()) },
	verticesCount{ static_cast<int>(vertices.size() / std::accumulate(attribSizes.begin(), attribSizes.end(), 0)) },
	usingEBO{ !indices.empty() },
	attribCount{ static_cast<int>(attribSizes.size()) }
{
	// Set up VAO and VBO
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

	// Set up EBO if indices provided
	if (usingEBO) {
		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
	}

	// set up attributes
	unsigned int stride{ 0 };
	for (unsigned int attribSize : attribSizes){
		stride += sizeof(float) * attribSize;
	}

	std::vector<size_t> offsets{ 0 };
	for (int i{ 1 }; i < attribSizes.size(); i++) {
		offsets.push_back(attribSizes[i - 1] * sizeof(float) + offsets[i - 1]);
	}

	for (int i{ 0 }; i < attribSizes.size(); i++) {
		glVertexAttribPointer(i, attribSizes[i], GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(offsets[i]));
		glEnableVertexAttribArray(i);
	}

	glBindVertexArray(0);
}

void Mesh::draw(const Shader& shader) const{
	activateTextures(shader);

	//draw mesh
	glBindVertexArray(VAO);

	if (usingEBO)
		glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, 0);
	else 
		glDrawArrays(GL_TRIANGLES, 0, verticesCount);


	glBindVertexArray(0);
}


void Mesh::drawInstanced(const Shader& shader, int count) const {
	activateTextures(shader);

	//draw instanced mesh
	glBindVertexArray(VAO);

	if (usingEBO)
		glDrawElementsInstanced(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, 0, count);
	else
		glDrawArraysInstanced(GL_TRIANGLES, 0, verticesCount, count);

	glBindVertexArray(0);
}

void Mesh::activateTextures(const Shader& shader) const {
	shader.use();

	int diffuseNum{ 1 }, specularNum{ 1 };

	// activate texture units binding textures and setting sampler uniforms in the shader to the unit number
	for (int i{ 0 }; i < textures.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i);

		// uniform name format : texture_typeN 
		std::string uniformName{ "texture_" };
		switch (textures[i].type) {

		case Texture::diffuse:
			uniformName += "diffuse" + std::to_string(diffuseNum++);
			break;
		case Texture::specular:
			uniformName += "specular" + std::to_string(specularNum++);
			break;
		}

		shader.setInt(uniformName, i);
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}

	glActiveTexture(GL_TEXTURE0);
}



unsigned int Mesh::getVAO() const {
	return VAO;
}

int Mesh::getAttribCount() const {
	return attribCount;
}