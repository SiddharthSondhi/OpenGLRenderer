#include "PhongMaterial.h"
#include <glad/glad.h>
#include <iostream>

PhongMaterial::PhongMaterial(unsigned int diffuseTex, unsigned int specularTex, unsigned int normalTex, unsigned int emissionTex,
							glm::vec3 diffuseColor, glm::vec3 specularColor, glm::vec3 emissionColor)
	: diffuseTex { diffuseTex },
	specularTex{ specularTex },
	normalTex{ normalTex },
	emissionTex{ emissionTex },
	diffuseColor {diffuseColor},
	specularColor{ specularColor },
	emissionColor{ emissionColor }
{}

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

	glActiveTexture(GL_TEXTURE0 + EMISSION_TEXTURE_UNIT);
	glBindTexture(GL_TEXTURE_2D, emissionTex);
	shader.setInt("material.texture_emission", EMISSION_TEXTURE_UNIT);

	shader.setVec3("material.diffuseColor", diffuseColor);
	shader.setVec3("material.specularColor", specularColor);
	shader.setVec3("material.emissionColor", emissionColor);

	shader.setVec2("material.textureScale", textureScale);
	shader.setBool("material.hasNormalMap", !(normalTex == 0));
	shader.setBool("material.hasSpecularMap", !(specularTex == 0));
	shader.setBool("material.hasEmissionMap", !(emissionTex == 0));
}

std::unique_ptr<Material> PhongMaterial::clone() const {
	return std::make_unique<PhongMaterial>(*this);
}

Material::Type PhongMaterial::getType() const {
	return Phong;
}


