#include "PointLight.h"

PointLight::PointLight(SceneObject obj, glm::vec3 color) 
	: obj{obj}, diffuse{color}, specular{color}
{}

