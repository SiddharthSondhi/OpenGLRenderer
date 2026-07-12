#include "PointLight.h"

PointLight::PointLight(SceneObject lightObj, glm::vec3 color) 
	: lightObj{lightObj}, color{color}
{}


void PointLight::draw(Shader& shader) const {
	shader.use();
	shader.setVec3("color", color);
	
	lightObj.draw(shader);
}
