#include "SolidColorMaterial.h"

SolidColorMaterial::SolidColorMaterial(const Shader& shader, const glm::vec3& color)
	: Material{ shader },
	  color{ color }
{}

std::unique_ptr<Material> SolidColorMaterial::clone() const {
	return std::make_unique<SolidColorMaterial>(*this);
}

void SolidColorMaterial::bind() const {
	shader->use();
	shader->setVec3("color", color);
}