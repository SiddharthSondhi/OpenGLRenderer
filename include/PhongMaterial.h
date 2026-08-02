#pragma once

#include "Texture.h"
#include "Material.h"

#include <glad/glad.h>


class PhongMaterial : public Material {
public:
	PhongMaterial(const Shader& shader, std::vector<Texture> textures = {});

	void bind() const override;
	std::unique_ptr<Material> clone() const override;

	glm::vec2 textureScale{ 1.0f };

private:
	std::vector<Texture> textures;
};