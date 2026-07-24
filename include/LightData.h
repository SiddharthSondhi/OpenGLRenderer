#pragma once

#include <glm/glm.hpp>
#include "Colors.h"

#include <array>

namespace GPUData {
	constexpr size_t MAX_NUMBER_POINT_LIGHTS{ 8 };

	struct DirLightData {
		glm::vec4 direction;
		glm::vec4 ambient{ Colors::white * .2f , 1.0f };
		glm::vec4 diffuse{ Colors::white * .7f , 1.0f };
		glm::vec4 specular{ Colors::white, 1.0f };
	};

	struct PointLightData {
		glm::vec4 position;

		//constant, linear, quadratic, unused
		glm::vec4 attenuation{ 1.0f, 0.09f, 0.032f, 0.0f };

		glm::vec4 ambient{ 0.0f };
		glm::vec4 diffuse{ 0.0f };
		glm::vec4 specular{ 0.0f };
	};

	struct SpotLightData {
		glm::vec4 position;
		glm::vec4 direction;

		// innerCutOff, outerCutOff, unused, unused
		glm::vec4 cutOffs{ glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(17.5f)), 0.0f, 0.0f };

		//constant, linear, quadratic, unused
		glm::vec4 attenuation{ 1.0f, 0.022f, 0.0019f, 0.0f };

		glm::vec4 diffuse{ Colors::white * .5f , 1.0f };
		glm::vec4 specular{ Colors::white , 1.0f };
	};


	struct LightData {
		DirLightData dirLight;
		SpotLightData spotLight;
		std::array<PointLightData, MAX_NUMBER_POINT_LIGHTS> pointLights;
		glm::vec4 enableFlashLight{ 0.0f };
	};
};