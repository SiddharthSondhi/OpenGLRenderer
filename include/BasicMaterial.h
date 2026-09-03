#pragma once

#include "Material.h"

class BasicMaterial : public Material {
public:
	BasicMaterial(Shader& shader);
	std::unique_ptr<Material> clone() const override;
	void bind(const Shader& shader) const override;
	Type getType() const override;
	Shader* shader;
};