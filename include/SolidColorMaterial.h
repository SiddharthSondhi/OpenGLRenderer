#pragma once

#include "Material.h"
#include "Shader.h"

class SolidColorMaterial : public Material {
public:
	SolidColorMaterial(const glm::vec3& color);

	std::unique_ptr<Material> clone() const override;
	void bind(const Shader& shader) const override;
	Type getType() const override;

private:
	glm::vec3 color;
};