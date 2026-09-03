#pragma once

#include "Shader.h"
#include <memory>


class Material {
public:
	enum Type {
		Phong,
		Basic,
		SolidColor
	};

	virtual ~Material() = default;

	virtual std::unique_ptr<Material> clone() const = 0;
	virtual void bind(const Shader& shader) const = 0;
	virtual Type getType() const = 0;
};