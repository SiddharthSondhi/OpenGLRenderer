#pragma once

#include "Material.h"

class PBRMaterial : public Material {
public:
	PBRMaterial(unsigned int albedoTex, unsigned int metallicTex = 0, unsigned int roughnessTex = 0, unsigned int AOtex = 0, unsigned int normalTex = 0,
		glm::vec3 albedoColor = glm::vec3(1.0f), float metallic = 0.5f, float roughness = 0.5f);

	PBRMaterial(glm::vec3 albedoColor, float metallic, float roughness);

	void bind(const Shader& shader) const override;
	std::unique_ptr<Material> clone() const override;
	Type getType() const override;

	unsigned int albedoTex{ 0 };
	unsigned int metallicTex{ 0 };
	unsigned int roughnessTex{ 0 };
	unsigned int AOTex{ 0 };
	unsigned int normalTex{ 0 };

	glm::vec3 albedoColor{1.0f};
	float metallic{ 0.5f };
	float roughness{ 0.5f };

	static constexpr unsigned int ALBEDO_TEXTURE_UNIT{ 0 };
	static constexpr unsigned int METALLIC_TEXTURE_UNIT{ 1 };
	static constexpr unsigned int ROUGHNESS_TEXTURE_UNIT{ 2 };
	static constexpr unsigned int AO_TEXTURE_UNIT{ 3 };
	static constexpr unsigned int NORMAL_TEXTURE_UNIT{ 4 };
};