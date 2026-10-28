#pragma once
#include "Scene.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <array>

namespace GUI {
	struct Options {
		//render path
		static constexpr std::array renderPaths{
			"forward",
			"deferred"
		};

		// scene
		static constexpr std::array scenes{
			"Main",
			"Planet",
			"City",
			"Country",
			"Reflective",
			"Light",
			"Shadows",
			"NormalMap",
			"HDR",
			"Backrooms",
			"PBR"
		};

		//skybox 
		static constexpr std::array skyboxOptions{
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

		// post processing filters
		static constexpr std::array postProcessingModes{
			"Regular",
			"Inverse", "Grey Scale",
			"Weighted Grey Scale",
			"Sharpen",
			"Emboss",
			"Test"
		};
		
		// tone mapping 
		static constexpr std::array toneMappingModes{
			"Off",
			"Rienhard",
			"Exposure"
		};
	};

	struct Settings {
		// -----------------MAIN SETTINGS-----------------
		// render path
		int currentRenderPath{ 1 };

		//scene
		int currentSceneIndex{ 0 };

		//skybox
		int currentSkyboxIndex{ 3 };

		//directional light 
		float dirLightYaw{ 0.0f };
		float dirLightPitch{ -30.0f };
		glm::vec3* dirLightDirection{ nullptr };
		bool directionalShadowsEnabled{ true };

		//post processing filters
		int postProcessingMode{ 0 };
		float convMatrixOffset{ 500.0f };

		//tone mapping
		int currentToneMapping{ 0 };
		float exposure{ 1.0f };

		// -----------------RENDER DEBUG-----------------
		//shadows
		unsigned int shadowMap{ 0 };

		//gBuffer
		unsigned int gPosition{ 0 };
		unsigned int gNormal{ 0 };
		unsigned int gAlbedoSpec{ 0 };
		unsigned int gEmission{ 0 };

		//SSAO
		unsigned int SSAOTexture{ 0 };
		unsigned int SSAOBlurTexture{ 0 };
		float AORadius{ 0.5 };
		float AOBias{ 0.025 };
		float AOPower{ 4 };
	};

	void init(GLFWwindow* window);
	void define();
	void render();
	void shutDown();
	void syncGui(Scene& scene);
}