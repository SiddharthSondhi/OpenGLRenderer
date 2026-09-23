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
		std::array<const char*, 9> skyboxOptions{ 
			"None",
			"Sky High Fluffy Cloud",
			"Planetary Earth",
			"Casual Day",
			"Day In The Clouds",
			"Dark Storm",
			"Space1",
			"Space2",
			"Space3"
		};

		int skyboxIndex{ 3 };

		// scene
		int currentSceneIndex{ 0 };
		std::array<const char*, 10> scenes{
			"Main",
			"Planet",
			"City",
			"Country",
			"Reflective",
			"Light",
			"Shadows",
			"NormalMap",
			"HDR",
			"Backrooms"
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

		int currentRenderPath{ 1 };

		//shadows
		unsigned int shadowMap{ 0 };

		//gBuffer
		unsigned int gPosition{ 0 };
		unsigned int gNormal{ 0 };
		unsigned int gAlbedoSpec{ 0 };
		unsigned int gEmission{ 0 };

		//SSAO
		unsigned int SSAOTexture{ 0 };
		float AORadius{ 0.5 };
		float AOBias{ 0.025 };
		float AOPower{ 4 };
	};

	void init(GLFWwindow* window);
	void define();
	void render();
	void shutDown();

	void settingsUI();
	void renderDebugUI();
}