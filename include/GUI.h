#pragma once
#include "Scene.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <array>


namespace GUI {
	struct Settings {
		// post processing 
		std::array<const char*, 7> postProcessingModes { 
			"Regular", 
			"Inverse", "Grey Scale", 
			"Weighted Grey Scale", 
			"Sharpen", 
			"Emboss", 
			"Test" 
		};
		int postProcessingMode{ 0 };
		float convMatrixOffset{ 500.0f };

		//skybox 
		std::array<const char*, 13> skyboxOptions{ 
			"None",
			"Sky High Fluffy Cloud",
			"Planetary Earth",
			"Mega Sun",
			"High Fantasy",
			"Under The Sea",
			"Casual Day",
			"Day In The Clouds",
			"Dark Storm",
			"CoriolisNight",
			"Space1",
			"Space2",
			"Space3"
		};

		int skyboxIndex{ 6 };

		// scene
		int currentSceneIndex{ 0 };
		std::array<const char*, 7> scenes{
			"Main",
			"Planet",
			"City",
			"Country",
			"Reflective",
			"Light",
			"Shadows"
		};

		//shadow map
		bool showShadowMap{ false };

		//directional light 
		glm::vec3* dirLightDirection{ nullptr };
	};

	void init(GLFWwindow* window);
	void define(Settings& gui);
	void render();
	void shutDown();
}