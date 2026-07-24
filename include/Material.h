#pragma once

#include "Shader.h"
#include <memory>


class Material {
public:
	Material(const Shader& shader);
	virtual ~Material() = default;

	virtual std::unique_ptr<Material> clone() const = 0;
	virtual void bind() const = 0;
	const Shader* getShader() const;

protected:
	const Shader* shader{ nullptr };
};