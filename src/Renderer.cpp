#include "Renderer.h"
#include "LightData.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

void Renderer::render(float windowWidth, float windowHeight, const Scene& scene, const Resources& resources) {
	//before rendering bind to the postProcFBO
	glBindFramebuffer(GL_FRAMEBUFFER, postProcFBO);
	postProcShader.use();
	postProcShader.setFloat("offset", 1.0f / gui.convMatrixOffset);
	postProcShader.setInt("postProcessingMode", gui.postProcessingMode);
	postProcShader.setInt("toneMappingMode", gui.currentToneMapping);
	postProcShader.setFloat("exposure", gui.exposure);

	//shadow pass
	shadowPass(scene, resources);

	//main pass
	mainPass(windowWidth, windowHeight, scene, resources);

	// now bind back to default framebuffer and draw a quad plane with the attached postProcFBO color texture
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.

	// clear all relevant buffers
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// draw screen quad
	glActiveTexture(GL_TEXTURE0);

	// if showShadowMap is enabled, draw it to the quad
	if (gui.showShadowMap) {
		depthDebugShader.use();
		glBindTexture(GL_TEXTURE_2D, shadowMap);
		depthDebugShader.setInt("depthMap", 0);
	}
	// otherwise draw the regular screen texture to the quad
	else {
		postProcShader.use();
		postProcShader.setInt("screenTexture", 0);
		glBindTexture(GL_TEXTURE_2D, postProcTextureColorBuffer);
	}

	screenQuadMesh.drawGeometry();
}

void Renderer::mainPass(float windowWidth, float windowHeight, const Scene& scene, const Resources& resources){
	// enable settings and clear bits for main render pass
	glViewport(0, 0, windowWidth, windowHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, postProcFBO);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	//update shadowMap in object shader before rendering
	resources.objectShader.use();
	glActiveTexture(GL_TEXTURE0 + SHADOW_MAP_TEXTURE_UNIT);
	resources.objectShader.setInt("shadowMap", SHADOW_MAP_TEXTURE_UNIT);
	glBindTexture(GL_TEXTURE_2D, shadowMap);

	// loop thorugh all point lights and draw them
	for (const PointLight& pointLight : scene.getPointLights()) {
		renderObject(pointLight.obj);
	}

	// loop through all scene objects and draw them
	for (const SceneObject& obj : scene.getSceneObjects()) {
		renderObject(obj);
	}

	//loop through all instanced object and draw them
	for (const InstancedSceneObject& obj : scene.getInstancedObjects()) {
		renderInstancedObject(obj);
	}

	// draw skybox after everything else
	renderSkyBox(resources.skyboxes[gui.skyboxIndex]);
}

void Renderer::shadowPass(const Scene& scene, const Resources& resources) {
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glEnable(GL_DEPTH_TEST);
	//glDisable(GL_CULL_FACE);
	//glCullFace(GL_FRONT);
	glClear(GL_DEPTH_BUFFER_BIT);

	// update shadow matrices
	float nearPlane{ 1.0f }, farPlane{ 40.0f };
	float size{ 20.0f };
	// swaped the sign of 3rd and 4th param to get rid of artifact from glCullFace(GL_FRONT) idk why??
	glm::mat4 dirLightProjection{ glm::ortho(-size, size, -size, size, nearPlane, farPlane) };
	glm::mat4 dirLightView = glm::lookAt(-10.0f * glm::normalize(scene.dirLight.direction), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	
	glm::mat4 dirLightSpaceMat{ dirLightProjection * dirLightView };

	shadowMapShader.use();
	shadowMapShader.setMat4("dirLightSpaceMat", dirLightSpaceMat);

	//also update in object shader
	resources.objectShader.use();
	resources.objectShader.setMat4("dirLightSpaceMat", dirLightSpaceMat);

	// render sceneObjects from light's perspective into shadow map
	for (const SceneObject& obj : scene.getSceneObjects()) {
		glm::mat4 modelMat{ obj.getModelMatrix() };

		const std::vector<Mesh>& meshes{ obj.getModel()->getMeshes() };
		for (const Mesh& mesh : meshes) {
			shadowMapShader.use();
			shadowMapShader.setMat4("model", obj.getModelMatrix());
			mesh.drawGeometry();
		}
	}
}

void Renderer::renderObject(const SceneObject& obj) const {
	glm::mat4 modelMat{ obj.getModelMatrix() };

	const std::vector<Mesh>& meshes{ obj.getModel()->getMeshes()};
	for (int i{ 0 }; i < meshes.size(); i++) {

		//choose shader based on material
		const Material* material{ nullptr };
		if (obj.getMaterialOverride()) {
			material = obj.getMaterialOverride();
		}
		else {
			material = obj.getModel()->getMaterials()[meshes[i].getMaterialIndex()].get();
		}

		const Shader* shader{ material->getShader() };

		if (!shader) {
			std::cout << "ERROR::NO SHADER SET\n";
			return;
		}

		shader->use();

		//update model matrix
		shader->setMat4("model", modelMat);

		//update normal matrix
		glm::mat3 normalMat = glm::transpose(glm::inverse(camera.getViewMatrix() * modelMat));
		shader->setMat3("normalMat", normalMat);

		material->bind();
		meshes[i].drawGeometry();
	}
}

void Renderer::renderInstancedObject(const InstancedSceneObject& obj) const {
	const std::vector<Mesh>& meshes{ obj.model->getMeshes() };
	for (const Mesh& mesh : meshes) {
		const Material& material{ *obj.model->getMaterials()[mesh.getMaterialIndex()] };
		const Shader* shader{ material.getShader() };

		if (!shader) {
			std::cout << "ERROR::NO SHADER SET\n";
			return;
		}

		shader->use();
		material.bind();
		mesh.drawInstancedGeometry(obj.count);
	}
}

void Renderer::renderSkyBox(unsigned int texture) {
	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);

	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_FALSE);

	glBindVertexArray(skybox.getVAO());
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);

	glBindVertexArray(0);
}

void Renderer::updateLightData(const Scene& scene, bool enableFlashLight) {
	GPUData::LightData lightData{};

	glm::mat4 view{ camera.getViewMatrix() };

	//directional light
	lightData.dirLight.direction = glm::normalize(view * glm::vec4{ scene.dirLight.direction, 0.0f });
	lightData.dirLight.ambient = glm::vec4{ scene.dirLight.ambient, 1.0f };
	lightData.dirLight.diffuse = glm::vec4{ scene.dirLight.diffuse, 1.0f };
	lightData.dirLight.specular = glm::vec4{ scene.dirLight.specular, 1.0f };

	// point lights
	const std::vector<PointLight>& pointLights{ scene.getPointLights() };
	for (size_t i{ 0 }; i < pointLights.size(); i++) {
		const PointLight& light{ pointLights[i] };

		lightData.pointLights[i].position = view * glm::vec4{ light.obj.position, 1.0f };

		lightData.pointLights[i].attenuation = glm::vec4{ light.constant, light.linear, light.quadratic, 1.0f };

		lightData.pointLights[i].ambient = glm::vec4{ light.ambient, 1.0f };
		lightData.pointLights[i].diffuse = glm::vec4{ light.diffuse, 1.0f };
		lightData.pointLights[i].specular = glm::vec4{ light.specular, 1.0f };
	}

	//spot light
	lightData.spotLight.position = glm::vec4{ 0.0f };
	lightData.spotLight.direction = glm::normalize(view * glm::vec4{ camera.front, 0.0f });
	lightData.enableFlashLight = enableFlashLight ? glm::vec4(1.0f) : glm::vec4(0.0f);

	glBindBuffer(GL_UNIFORM_BUFFER, lightDataUBO);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(GPUData::LightData), reinterpret_cast<void*>(&lightData));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}


void Renderer::createLightDataUBO() {
	glGenBuffers(1, &lightDataUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, lightDataUBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(GPUData::LightData), NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, lightDataUBO);
}

void Renderer::createMatricesUBO() {
	glGenBuffers(1, &matricesUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, matricesUBO);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, matricesUBO);
}

void Renderer::setUpPostProcessing(float windowWidth, float windowHeight) {
	//postProcFBO
	glGenFramebuffers(1, &postProcFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, postProcFBO);

	// generate texture
	glGenTextures(1, &postProcTextureColorBuffer);
	glBindTexture(GL_TEXTURE_2D, postProcTextureColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, windowWidth, windowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	// attach it to currently bound postProcFBO object
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postProcTextureColorBuffer, 0);

	//renderbuffer for depth/stencil testing
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, windowWidth, windowHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// attach it to currently bound postProcFBO object
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer for postprocessing is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::updateMatrices(float windowWidth, float windowHeight) const {
	// calculate view and projection matrix
	glm::mat4 view{ camera.getViewMatrix() };
	glm::mat4 projection{ glm::perspective(glm::radians(camera.zoom), static_cast<float>(windowWidth) / windowHeight, 0.1f, 500.0f) };

	glBindBuffer(GL_UNIFORM_BUFFER, matricesUBO);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(view));
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projection));
}

void Renderer::setUpShadowMap() {
	glGenFramebuffers(1, &shadowMapFBO);
	
	glGenTextures(1, &shadowMap);
	glBindTexture(GL_TEXTURE_2D, shadowMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer for shadowmap is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void Renderer::init(float windowWidth, float windowHeight) {
	setUpPostProcessing(windowWidth, windowHeight);
	createLightDataUBO();
	createMatricesUBO();
	setUpShadowMap();
}
