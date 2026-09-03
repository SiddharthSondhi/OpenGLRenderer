#pragma once

#include "Texture.h"
#include "Material.h"

#include <glad/glad.h>


class PhongMaterial : public Material {
public:
	PhongMaterial(std::vector<Texture> textures = {}, float shininess = 256.0f);

	void bind(const Shader& shader) const override;
	std::unique_ptr<Material> clone() const override;
	Type getType() const override;

	glm::vec2 textureScale{ 1.0f };
	bool hasNormalMap{ false };
	float shininess;

	unsigned int textureDiffuse{ 0 };
	unsigned int textureSpecular{ 0 };
	unsigned int textureNormal{ 0 };
};