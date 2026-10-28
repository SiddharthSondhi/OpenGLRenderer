#include "GUI.h"

#include <glm/gtc/type_ptr.hpp>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

extern GUI::Settings gui;

namespace GUI {

	static void settingsUI();
	static void renderDebugUI();

	static void syncDirectionalLightControls(Scene& scene);
	static void updateSceneLightDirection();

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
		ImGui::Combo("Render Path", &gui.currentRenderPath, Options::renderPaths.data(), static_cast<int>(Options::renderPaths.size()));

		ImGui::Combo("Scene", &gui.currentSceneIndex, Options::scenes.data(), static_cast<int>(Options::scenes.size()));
		ImGui::Combo("SkyBox Texture", &gui.currentSkyboxIndex, Options::skyboxOptions.data(), static_cast<int>(Options::skyboxOptions.size()));


		ImGui::Text("Directional Light");
		bool changedDirLightDirection{ false };
		changedDirLightDirection |= ImGui::SliderFloat("Yaw", &gui.dirLightYaw, -180.0f, 180.0f);
		changedDirLightDirection |= ImGui::SliderFloat("Pitch", &gui.dirLightPitch, -90.0f, 90.0f);
		if (changedDirLightDirection && gui.dirLightDirection) {
			updateSceneLightDirection();
		}

		ImGui::Checkbox("Enable Shadows", &gui.directionalShadowsEnabled);

		ImGui::Text("SSAO");
		ImGui::SliderFloat("Radius", &gui.AORadius, 0.1f, 10.0f);
		ImGui::SliderFloat("Bias", &gui.AOBias, 0.0f, 1.0f);
		ImGui::SliderFloat("Power", &gui.AOPower, 0.1f, 20.0f);

		ImGui::Text("Post Processing");
		ImGui::Combo("Post-Processing Mode", &gui.postProcessingMode, Options::postProcessingModes.data(), static_cast<int>(Options::postProcessingModes.size()));
		ImGui::SliderFloat("1 / offset", &gui.convMatrixOffset, 1.0f, 5000.0f);

		ImGui::Combo("Tone Mapping", &gui.currentToneMapping, Options::toneMappingModes.data(), static_cast<int>(Options::toneMappingModes.size()));
		if (gui.currentToneMapping == 2){
			ImGui::SliderFloat("Exposure", &gui.exposure, 0.0f, 5.0f);
		}

		ImGui::End();
	}

	void renderDebugUI() {
		ImGui::Begin("Render Debug");
		constexpr float DEBUG_WIDTH{ 480 };
		constexpr float DEBUG_HEIGHT{ 270 };

		if (ImGui::CollapsingHeader("Shadows")){
			ImGui::Text("Directional Shadow Map");
			ImGui::Image(static_cast<ImTextureID>(gui.shadowMap), ImVec2(DEBUG_WIDTH, DEBUG_HEIGHT), ImVec2(0, 1), ImVec2(1, 0));
		}


		if (ImGui::CollapsingHeader("G-Buffer")){
			ImGui::Text("Position");
			ImGui::Image(static_cast<ImTextureID>(gui.gPosition), ImVec2(DEBUG_WIDTH, DEBUG_HEIGHT), ImVec2(0, 1), ImVec2(1, 0));

			ImGui::Text("Normal");
			ImGui::Image(static_cast<ImTextureID>(gui.gNormal), ImVec2(DEBUG_WIDTH, DEBUG_HEIGHT), ImVec2(0, 1), ImVec2(1, 0));

			ImGui::Text("Albedo / Specular");
			ImGui::Image(static_cast<ImTextureID>(gui.gAlbedoSpec),ImVec2(DEBUG_WIDTH, DEBUG_HEIGHT), ImVec2(0, 1), ImVec2(1, 0));

			ImGui::Text("Emission");
			ImGui::Image(static_cast<ImTextureID>(gui.gEmission), ImVec2(DEBUG_WIDTH, DEBUG_HEIGHT), ImVec2(0, 1), ImVec2(1, 0));
		}

		if (ImGui::CollapsingHeader("SSAO")) {
			ImGui::Text("SSAO");
			ImGui::Image(static_cast<ImTextureID>(gui.SSAOTexture), ImVec2(DEBUG_WIDTH, DEBUG_HEIGHT), ImVec2(0, 1), ImVec2(1, 0));

			ImGui::Text("SSAO Blurred");
			ImGui::Image(static_cast<ImTextureID>(gui.SSAOBlurTexture), ImVec2(DEBUG_WIDTH, DEBUG_HEIGHT), ImVec2(0, 1), ImVec2(1, 0));
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

	void syncGui(Scene& scene) {
		syncDirectionalLightControls(scene);
	}

	void syncDirectionalLightControls(Scene& scene) {
		gui.dirLightDirection = &scene.dirLight.direction;
		glm::vec3 dir = glm::normalize(*gui.dirLightDirection);

		gui.dirLightPitch = glm::degrees(std::asin(glm::clamp(dir.y, -1.0f, 1.0f)));
		gui.dirLightYaw = glm::degrees(std::atan2(dir.z, dir.x));
	}

	void updateSceneLightDirection() {
		float yaw{ glm::radians(gui.dirLightYaw) };
		float pitch{ glm::radians(gui.dirLightPitch) };

		glm::vec3 direction{
			cos(yaw) * cos(pitch),
			sin(pitch),
			sin(yaw) * cos(pitch)
		};

		*gui.dirLightDirection = direction;
	}
}