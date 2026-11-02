#include "PBRMaterial.h"
#include <glad/glad.h>
#include <memory>

PBRMaterial::PBRMaterial(unsigned int albedoTex, unsigned int metallicTex, unsigned int roughnessTex, 
						 unsigned int AOTex, unsigned int normalTex, unsigned int metallicRoughnessTex,
						 glm::vec3 albedoColor, float metallic, float roughness)
	: albedoTex{ albedoTex },
	metallicTex{ metallicTex },
	roughnessTex{ roughnessTex },
	AOTex{ AOTex },
	normalTex{ normalTex },
	metallicRoughnessTex{ metallicRoughnessTex },
	albedoColor{ albedoColor },
	metallic{ metallic },
	roughness{ roughness } 
{}

PBRMaterial::PBRMaterial(glm::vec3 albedoColor, float metallic, float roughness)
	: albedoColor{ albedoColor },
	metallic{ metallic },
	roughness{ roughness }
{}


void PBRMaterial::bind(const Shader& shader) const {
	shader.use();

	// bind textures and set uniforms
	glActiveTexture(GL_TEXTURE0 + ALBEDO_TEXTURE_UNIT);
	glBindTexture(GL_TEXTURE_2D, albedoTex);
	shader.setInt("pbrMat.texture_albedo", ALBEDO_TEXTURE_UNIT);

	glActiveTexture(GL_TEXTURE0 + METALLIC_TEXTURE_UNIT);
	glBindTexture(GL_TEXTURE_2D, metallicTex);
	shader.setInt("pbrMat.texture_metallic", METALLIC_TEXTURE_UNIT);

	glActiveTexture(GL_TEXTURE0 + ROUGHNESS_TEXTURE_UNIT);
	glBindTexture(GL_TEXTURE_2D, roughnessTex);
	shader.setInt("pbrMat.texture_roughness", ROUGHNESS_TEXTURE_UNIT);

	glActiveTexture(GL_TEXTURE0 + AO_TEXTURE_UNIT);
	glBindTexture(GL_TEXTURE_2D, AOTex);
	shader.setInt("pbrMat.texture_AO", AO_TEXTURE_UNIT);

	glActiveTexture(GL_TEXTURE0 + NORMAL_TEXTURE_UNIT);
	glBindTexture(GL_TEXTURE_2D, normalTex);
	shader.setInt("pbrMat.texture_normal", NORMAL_TEXTURE_UNIT);

	glActiveTexture(GL_TEXTURE0 + METALLIC_ROUGHNESS_TEXTURE_UNIT);
	glBindTexture(GL_TEXTURE_2D, metallicRoughnessTex);
	shader.setInt("pbrMat.texture_metallic_roughness", METALLIC_ROUGHNESS_TEXTURE_UNIT);

	shader.setVec3("pbrMat.albedoColor", albedoColor);
	shader.setFloat("pbrMat.metallic", metallic);
	shader.setFloat("pbrMat.roughness", roughness);

	shader.setBool("pbrMat.hasAlbedoMap", !(albedoTex == 0));
	shader.setBool("pbrMat.hasMetallicMap", !(metallicTex == 0));
	shader.setBool("pbrMat.hasRoughnessMap", !(roughnessTex == 0));
	shader.setBool("pbrMat.hasAOMap", !(AOTex == 0));
	shader.setBool("pbrMat.hasNormalMap", !(normalTex == 0));
	shader.setBool("pbrMat.packedMetallicRoughnessTex", !(metallicRoughnessTex == 0));
}

std::unique_ptr<Material> PBRMaterial::clone() const {
	return std::make_unique<PBRMaterial>(*this);
}

Material::Type PBRMaterial::getType() const {
	return PBR;
}