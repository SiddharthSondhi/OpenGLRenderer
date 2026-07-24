#pragma once

#include "Scene.h"
#include "LightData.h"
#include "Camera.h"
#include "InstancedSceneObject.h"
#include "GUI.h"
#include "VertexData.h"
#include "Resources.h"


extern Camera camera;
extern GUI::Settings gui;

class Renderer {
public:
	void render(const Scene& scene, Resources& resources);

	void createLightDataUBO();
	void updateLightData(const Scene& scene, bool enableFlashLight);
	void uploadLightData();

	void createMatricesUBO();
	void updateAndUploadMatrices(float windowWidth, float windowHeight) const;

	void setUpPostProcessing(float windowWidth, float windowHeight);

private:
	// light data
	unsigned int lightDataUBO;
	GPUData::LightData lightData;

	//matrices data
	unsigned int matricesUBO;

	// post processing
	Shader frameBufferShader{ "./shaders/frameBufferVS.glsl", "./shaders/frameBufferFS.glsl" };
	Mesh screenQuadMesh{ VertexData::screenQuad, {2, 2} };
	unsigned int framebuffer{ 0 };
	unsigned int textureColorbuffer{ 0 };
	
	// rendering
	void renderObject(const SceneObject& obj) const;
	void renderInstancedObject(const InstancedSceneObject& obj) const;

	//skybox
	void renderSkyBox(unsigned int texture);
	Shader skyboxShader{ "./shaders/skyboxVS.glsl", "./shaders/skyboxFS.glsl" };
	Mesh skybox{ VertexData::skyboxVertices, { 3 } };
};