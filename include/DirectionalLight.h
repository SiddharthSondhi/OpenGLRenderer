#pragma once

#include <glm/glm.hpp>
#include <Colors.h>

class DirectionalLight {
public:
	glm::vec3 direction{ -0.3f, -1.0f, -0.2f };
	glm::vec3 color { Colors::black };


	DirectionalLight() = default;

	DirectionalLight(glm::vec3 color) 
		: color { color } 
	{}

	DirectionalLight(glm::vec3 direction, glm::vec3 color)
		: direction{ direction }, color{ color } 
	{}
};