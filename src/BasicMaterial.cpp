#include "BasicMaterial.h"

BasicMaterial::BasicMaterial(const Shader& shader)
	: Material{ shader }
{}

std::unique_ptr<Material> BasicMaterial::clone() const {
	return std::make_unique<BasicMaterial>(*this);
}

void BasicMaterial::bind() const {

}
