#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <array>


namespace GUI {
	struct Settings {
		// post processing options
		std::array<const char*, 7> postProcessingModes { "Regular", "Inverse", "Grey Scale", "Weighted Grey Scale", "Sharpen", "Emboss", "Test" };
		int postProcessingMode{ 0 };
		float convMatrixOffset{ 500.0f };
	};

	void init(GLFWwindow* window);
	void define(Settings& gui);
	void render();
	void shutDown();
}