#include "BasicMaterial.h"

BasicMaterial::BasicMaterial(Shader& shader) 
	: shader{ &shader }
{}

std::unique_ptr<Material> BasicMaterial::clone() const {
	return std::make_unique<BasicMaterial>(*this);
}

void BasicMaterial::bind(const Shader& shader) const {
	this->shader->use();
}

Material::Type BasicMaterial::getType() const {
	return Basic;
}
