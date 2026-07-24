#include "Material.h"
#include "Shader.h"
#include <glad/glad.h>

Material::Material(const Shader& shader) 
	: shader{&shader}
{}

const Shader* Material::getShader() const {
	return shader;
}


