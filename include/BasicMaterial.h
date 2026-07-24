#pragma once

#include "Material.h"

class BasicMaterial : public Material {
public:
	BasicMaterial(const Shader& shader);
	std::unique_ptr<Material> clone() const override;
	void bind() const override;
};