#pragma once

#include "SceneObject.h"
#include "Shader.h"

class PointLight {
public:
	SceneObject obj;
	glm::vec3 color;

	PointLight(SceneObject obj, glm::vec3 color);
};