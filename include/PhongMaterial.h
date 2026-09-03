#pragma once

#include "Material.h"

#include <glad/glad.h>


class PhongMaterial : public Material {
public:
	PhongMaterial(unsigned int diffuseTex, unsigned int specularTex = 0, unsigned int normalTex = 0, float shininess = 256.0f);

	void bind(const Shader& shader) const override;
	std::unique_ptr<Material> clone() const override;
	Type getType() const override;

	glm::vec2 textureScale{ 1.0f };
	float shininess;

	unsigned int diffuseTex;
	unsigned int specularTex;
	unsigned int normalTex;

	static constexpr unsigned int DIFFUSE_TEXTURE_UNIT{ 0 };
	static constexpr unsigned int SPECULAR_TEXTURE_UNIT{ 1 };
	static constexpr unsigned int NORMAL_TEXTURE_UNIT{ 2 };
};