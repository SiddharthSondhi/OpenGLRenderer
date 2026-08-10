#pragma once

#include "SceneObject.h"
#include "Shader.h"
#include "Colors.h"

class PointLight {
public:
	SceneObject obj;
	glm::vec3 ambient{ Colors::black };
	glm::vec3 diffuse{ Colors::white };
	glm::vec3 specular{ Colors::white };

	// attenuation
	float constant{ 1.0f };
	float linear{ 0.09f };
	float quadratic{ 0.032f };

	PointLight(SceneObject obj, glm::vec3 color);
};