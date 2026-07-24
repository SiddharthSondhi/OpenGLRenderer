#include "PhongMaterial.h"
#include <glad/glad.h>

PhongMaterial::PhongMaterial(const Shader& shader, std::vector<Texture> textures)
	: Material{ shader }, textures{ textures }
{}

void PhongMaterial::bind() const{
	shader->use();
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

		shader->setInt(uniformName, i);
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}

	glActiveTexture(GL_TEXTURE0);
}

std::unique_ptr<Material> PhongMaterial::clone() const {
	return std::make_unique<PhongMaterial>(*this);
}

