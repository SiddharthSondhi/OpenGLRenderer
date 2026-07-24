#pragma once

#include "Material.h"
#include "Shader.h"

class SolidColorMaterial : public Material {
public:
	SolidColorMaterial(const Shader& shader, const glm::vec3& color);

	std::unique_ptr<Material> clone() const override;
	void bind() const override;

private:
	glm::vec3 color;
};