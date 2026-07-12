#pragma once

#include <glm/glm.hpp>
#include <Colors.h>

class DirectionalLight {
public:
	glm::vec3 direction{ -0.3f, -1.0f, -0.2f };
	glm::vec3 ambient { Colors::black };
	glm::vec3 diffuse { Colors::black };
	glm::vec3 specular{ Colors::black };

	DirectionalLight() = default;

	DirectionalLight(glm::vec3 Color) {
		ambient = Color * .2f;
		diffuse = Color * .7f;
		specular = Color;
	}

	DirectionalLight(glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular)
		: direction{ direction }, ambient{ ambient }, diffuse{ diffuse }, specular{ specular } {}
};