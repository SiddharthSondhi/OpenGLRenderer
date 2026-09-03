#include "GUI.h"

#include <glm/gtc/type_ptr.hpp>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

extern GUI::Settings gui;

namespace GUI {
	void init(GLFWwindow* window){
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();

		// Style
		ImGui::StyleColorsDark();

		// Initialize ImGui for GLFW + OpenGL
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 460");
	}

	void define() {
		// Start the ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		
		settingsUI();
		renderDebugUI();
	}

	void settingsUI(){
		ImGui::Begin("Settings");

		// specify UI elements
		ImGui::Text("Frame time: %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		ImGui::Combo("Render Path", &gui.currentRenderPath, gui.renderPaths.data(), static_cast<int>(gui.renderPaths.size()));

		ImGui::Combo("Post-Processing Mode", &gui.postProcessingMode, gui.postProcessingModes.data(), static_cast<int>(gui.postProcessingModes.size()));
		ImGui::SliderFloat("1 / offset", &gui.convMatrixOffset, 1.0f, 5000.0f);

		ImGui::Combo("SkyBox Texture", &gui.skyboxIndex, gui.skyboxOptions.data(), static_cast<int>(gui.skyboxOptions.size()));
		ImGui::Combo("Scene", &gui.currentSceneIndex, gui.scenes.data(), static_cast<int>(gui.scenes.size()));

		if (gui.dirLightDirection)
			ImGui::DragFloat3("Directional Light Direction", glm::value_ptr(*gui.dirLightDirection), 0.01f, -10.0f, 10.0f);

		ImGui::Combo("Tone Mapping", &gui.currentToneMapping, gui.toneMappingModes.data(), static_cast<int>(gui.toneMappingModes.size()));
		if (gui.currentToneMapping == 2)
		{
			ImGui::SliderFloat("Exposure", &gui.exposure, 0.0f, 5.0f);
		}

		ImGui::End();
	}

	void renderDebugUI() {
		ImGui::Begin("Render Debug");
		float width{ 480 }, height{ 270 };

		if (ImGui::CollapsingHeader("Shadows")){
			ImGui::Text("Directional Shadow Map");
			ImGui::Image(static_cast<ImTextureID>(gui.shadowMap), ImVec2(width, height), ImVec2(0, 1), ImVec2(1, 0));
		}


		if (ImGui::CollapsingHeader("G-Buffer"))
		{
			ImGui::Text("Position");
			ImGui::Image(static_cast<ImTextureID>(gui.gPosition), ImVec2(width, height), ImVec2(0, 1), ImVec2(1, 0));

			ImGui::Text("Normal");
			ImGui::Image(static_cast<ImTextureID>(gui.gNormal),ImVec2(width, height), ImVec2(0, 1), ImVec2(1, 0));

			ImGui::Text("Albedo / Specular");
			ImGui::Image(static_cast<ImTextureID>(gui.gAlbedoSpec),ImVec2(width, height), ImVec2(0, 1), ImVec2(1, 0));
		}
		
		ImGui::End();
	}

	void render() {
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void shutDown() {
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}
	
}