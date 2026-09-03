#include "PhongMaterial.h"
#include <glad/glad.h>
#include <iostream>

PhongMaterial::PhongMaterial(unsigned int diffuseTex, unsigned int specularTex, unsigned int normalTex, float shininess)
	: diffuseTex { diffuseTex },
	specularTex{ specularTex },
	normalTex{ normalTex },
	shininess{ shininess } {}

void PhongMaterial::bind(const Shader& shader) const{
	shader.use();

	// bind textures and set uniforms
	glActiveTexture(GL_TEXTURE0 + DIFFUSE_TEXTURE_UNIT);
	glBindTexture(GL_TEXTURE_2D, diffuseTex);
	shader.setInt("material.texture_diffuse", DIFFUSE_TEXTURE_UNIT);

	glActiveTexture(GL_TEXTURE0 + SPECULAR_TEXTURE_UNIT);
	glBindTexture(GL_TEXTURE_2D, specularTex);
	shader.setInt("material.texture_specular", SPECULAR_TEXTURE_UNIT);

	glActiveTexture(GL_TEXTURE0 + NORMAL_TEXTURE_UNIT);
	glBindTexture(GL_TEXTURE_2D, normalTex);
	shader.setInt("material.texture_normal", NORMAL_TEXTURE_UNIT);

	shader.setVec2("material.textureScale", textureScale);
	shader.setBool("material.hasNormalMap", !(normalTex == 0));
	shader.setBool("material.hasSpecularMap", !(specularTex == 0));

	shader.setFloat("material.shininess", shininess);
}

std::unique_ptr<Material> PhongMaterial::clone() const {
	return std::make_unique<PhongMaterial>(*this);
}

Material::Type PhongMaterial::getType() const {
	return Phong;
}


