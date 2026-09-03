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
		std::array<const char*, 9> scenes{
			"Main",
			"Planet",
			"City",
			"Country",
			"Reflective",
			"Light",
			"Shadows",
			"NormalMap",
			"HDR"
		};

		//directional light 
		glm::vec3* dirLightDirection{ nullptr };

		//tone mapping
		int currentToneMapping{ 0 };
		std::array<const char*, 3> toneMappingModes{
			"Off",
			"Rienhard",
			"Exposure"
		};

		float exposure{ 1.0f };

		//render path
		std::array<const char*, 2> renderPaths {
			"forward",
			"deferred"
		};

		int currentRenderPath{ 0 };

		//shadows
		unsigned int shadowMap{ 0 };

		//gBuffer
		unsigned int gPosition{ 0 };
		unsigned int gNormal{ 0 };
		unsigned int gAlbedoSpec{ 0 };
	};

	void init(GLFWwindow* window);
	void define();
	void render();
	void shutDown();

	void settingsUI();
	void renderDebugUI();
}