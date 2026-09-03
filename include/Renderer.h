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
	void render(float windowWidth, float windowHeight, const Scene& scene, const Resources& resources);
	void init(float windowWidth, float windowHeight);

	void updateLightData(const Scene& scene, bool enableFlashLight);
	void updateMatrices(float windowWidth, float windowHeight) const;	

private:
	// UBOs
	unsigned int matricesUBO;
	void createMatricesUBO();
	static constexpr unsigned int MATRICES_UBO_BINDING{ 0 };

	unsigned int lightDataUBO;
	void createLightDataUBO();
	static constexpr unsigned int LIGHT_DATA_UBO_BINDING{ 1 };


	// post processing
	Shader postProcShader{ "./shaders/frameBufferVS.glsl", "./shaders/frameBufferFS.glsl" };
	Mesh screenQuadMesh{ VertexData::screenQuad, {2, 2} };
	unsigned int postProcFBO{ 0 };
	unsigned int postProcTextureColorBuffer{ 0 };
	void setUpPostProcessing(float windowWidth, float windowHeight);
	void renderPostProcessing() const;

	//skybox
	void renderSkyBox(unsigned int texture);
	Shader skyboxShader{ "./shaders/skyboxVS.glsl", "./shaders/skyboxFS.glsl" };
	Mesh skybox{ VertexData::skyboxVertices, { 3 } };

	//shadows
	Shader shadowMapShader{ "./shaders/shadowMapVS.glsl", "./shaders/shadowMapFS.glsl" };
	Shader depthDebugShader{ "./shaders/depthDebugVS.glsl", "./shaders/depthDebugFS.glsl" };
	void setUpShadowMap();
	void shadowPass(const Scene& scene, const Resources& resources);
	unsigned int shadowMapFBO;
	unsigned int shadowMap;
	static constexpr unsigned int SHADOW_WIDTH = 4096;
	static constexpr unsigned int SHADOW_HEIGHT = 4096;
	static constexpr unsigned int SHADOW_MAP_TEXTURE_UNIT = 3;
	glm::mat4 dirLightSpaceMat;


	// forward rendering
	void renderForward(float windowWidth, float windowHeight, const Scene& scene, const Resources& resrouces);
	void renderObjectForward(const SceneObject& obj, const Resources& resources) const;
	void renderInstancedObject(const InstancedSceneObject& obj, const Resources& resources) const;

	// deferred rendering
	unsigned int gBuffer;
	unsigned int gPosition;
	unsigned int gNormal;
	unsigned int gAlbedoSpec;
	void renderDeferred(float windowWidth, float windowHeight, const Scene& scene, const Resources& resources);
	void renderObjectDeferred(const SceneObject& obj) const;
	void setUpGBuffer(float windowWidth, float windowHeight);
	Shader gBufferShader{"./shaders/gBufferVS.glsl", "./shaders/gBufferFS.glsl" };

	// rendering helper
	const Material* getMaterial(const SceneObject& obj, const Mesh& mesh) const;
};