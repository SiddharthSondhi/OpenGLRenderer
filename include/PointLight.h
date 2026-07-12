#pragma once

#include "SceneObject.h"
#include "Shader.h"

class PointLight {
public:
	SceneObject lightObj;
	glm::vec3 color;

	PointLight(SceneObject lightObj, glm::vec3 color);
	void draw(Shader& shader) const;
};