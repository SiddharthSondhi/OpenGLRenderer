#include "PhongMaterial.h"
#include <glad/glad.h>
#include <iostream>

PhongMaterial::PhongMaterial(std::vector<Texture> textures, float shininess)
	: textures{ textures } , shininess {shininess}
{
	for (const Texture& t : textures) {
		if (t.type == Texture::normal)
			hasNormalMap = true;
	}
}

void PhongMaterial::bind(const Shader& shader) const{
	int diffuseNum{ 1 }, specularNum{ 1 }, normalNum{ 1 };

	// unbind previous textures
	for (int i{ 0 }; i < 5; i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	// activate texture units binding textures and setting sampler uniforms in the shader to the unit number
	for (int i{ 0 }; i < textures.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i);

		// uniform name format : texture_typeN 
		std::string uniformName{ "material.texture_" };
		switch (textures[i].type) {

		case Texture::diffuse:
			uniformName += "diffuse" + std::to_string(diffuseNum++);
			break;
		case Texture::specular:
			uniformName += "specular" + std::to_string(specularNum++);
			break;
		case Texture::normal:
			uniformName += "normal" + std::to_string(normalNum++);
			break;
		}

		shader.setInt(uniformName, i);
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}

	glActiveTexture(GL_TEXTURE0);

	shader.setVec2("material.textureScale", textureScale);
	shader.setBool("material.hasNormalMap", hasNormalMap);
	shader.setFloat("material.shininess", shininess);
}

std::unique_ptr<Material> PhongMaterial::clone() const {
	return std::make_unique<PhongMaterial>(*this);
}

Material::Type PhongMaterial::getType() const {
	return Phong;
}


