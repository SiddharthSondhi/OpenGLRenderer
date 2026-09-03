#include "SolidColorMaterial.h"

SolidColorMaterial::SolidColorMaterial( const glm::vec3& color) 
	: color{ color } {}

std::unique_ptr<Material> SolidColorMaterial::clone() const {
	return std::make_unique<SolidColorMaterial>(*this);
}

void SolidColorMaterial::bind(const Shader& shader) const {
	shader.use();
	shader.setVec3("color", color);
}

Material::Type SolidColorMaterial::getType() const {
	return SolidColor;
}