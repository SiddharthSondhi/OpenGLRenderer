#include "Renderer.h"
#include "LightData.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <random>
#include <cmath>

void Renderer::render(float windowWidth, float windowHeight, const Scene& scene, const Resources& resources) {
	//shadow pass
	shadowPass(scene, resources);

	//main pass
	if (gui.currentRenderPath == 0)
		renderForward(windowWidth, windowHeight, scene, resources);
	else if (gui.currentRenderPath == 1)
		renderDeferred(windowWidth, windowHeight, scene, resources);

	//render skybox last
	renderSkyBox(resources.skyboxes[gui.skyboxIndex]);

	renderPostProcessing();
}

void Renderer::renderPostProcessing() const {
	// now bind back to default framebuffer and draw a quad plane with the attached postProcFBO color texture
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test

	// clear all relevant buffers
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// draw screen quad
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, postProcTextureColorBuffer);

	postProcShader.use();
	postProcShader.setFloat("offset", 1.0f / gui.convMatrixOffset);
	postProcShader.setInt("postProcessingMode", gui.postProcessingMode);
	postProcShader.setInt("toneMappingMode", gui.currentToneMapping);
	postProcShader.setFloat("exposure", gui.exposure);
	screenQuadMesh.drawGeometry();
}


void Renderer::renderDeferred(float windowWidth, float windowHeight, const Scene& scene, const Resources& resources){
	// geometry pass
	glViewport(0, 0, windowWidth, windowHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glDisable(GL_BLEND);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	//loop through all scene objects, if using a supported Material, then draw them to gBuffer
	for (const SceneObject& obj : scene.getSceneObjects()) {
		renderObjectDeferred(obj);
	}

	//SSAO Pass
	glBindFramebuffer(GL_FRAMEBUFFER, SSAOFBO);
	glClear(GL_COLOR_BUFFER_BIT);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, SSAONoiseTexture);
	
	SSAOShader.use();
	
	SSAOShader.setFloat("radius", gui.AORadius);
	SSAOShader.setFloat("bias", gui.AOBias);

	screenQuadMesh.drawGeometry();

	//lighting pass
	glBindFramebuffer(GL_FRAMEBUFFER, postProcFBO);
	
	glDisable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, gEmission);

	glActiveTexture(GL_TEXTURE0 + SHADOW_MAP_TEXTURE_UNIT);
	glBindTexture(GL_TEXTURE_2D, shadowMap);

	glActiveTexture(GL_TEXTURE0 + SSAO_TEXTURE_UNIT);
	glBindTexture(GL_TEXTURE_2D, SSAOColorBuffer);

	resources.deferredPhongShader.use();

	resources.deferredPhongShader.setMat4("viewToDirLightSpaceMat", dirLightSpaceMat * glm::inverse(camera.getViewMatrix()));
	resources.deferredPhongShader.setFloat("AOPower", gui.AOPower);

	screenQuadMesh.drawGeometry();

	//copy depth buffer into postProcFBO
	glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, postProcFBO);
	glBlitFramebuffer(0, 0, windowWidth, windowHeight, 0, 0, windowWidth, windowHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

	//render rest of objects with normal forward rendering
	glEnable(GL_DEPTH_TEST);
	for (const SceneObject& obj : scene.getSceneObjects()) {
		renderObjectDeferredForward(obj, resources);
	}

	// loop thorugh all point lights and draw them
	for (const PointLight& pointLight : scene.getPointLights()) {
		renderObjectForward(pointLight.obj, resources);
	}

	//loop through all instanced object and draw them
	for (const InstancedSceneObject& obj : scene.getInstancedObjects()) {
		renderInstancedObject(obj, resources);
	}
}

void Renderer::renderForward(float windowWidth, float windowHeight, const Scene& scene, const Resources& resources){
	// enable settings and clear bits for main render pass
	glViewport(0, 0, windowWidth, windowHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, postProcFBO);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glDisable(GL_BLEND);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	//update shadowMap in object shader before rendering
	resources.phongShader.use();
	glActiveTexture(GL_TEXTURE0 + SHADOW_MAP_TEXTURE_UNIT);
	resources.phongShader.setInt("shadowMap", SHADOW_MAP_TEXTURE_UNIT);
	glBindTexture(GL_TEXTURE_2D, shadowMap);

	// loop thorugh all point lights and draw them
	for (const PointLight& pointLight : scene.getPointLights()) {
		renderObjectForward(pointLight.obj, resources);
	}

	// loop through all scene objects and draw them
	for (const SceneObject& obj : scene.getSceneObjects()) {
		renderObjectForward(obj, resources);
	}

	//loop through all instanced object and draw them
	for (const InstancedSceneObject& obj : scene.getInstancedObjects()) {
		renderInstancedObject(obj, resources);
	}
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
	
	dirLightSpaceMat = dirLightProjection * dirLightView;

	shadowMapShader.use();
	shadowMapShader.setMat4("dirLightSpaceMat", dirLightSpaceMat);

	//also update in object shader
	resources.phongShader.use();
	resources.phongShader.setMat4("dirLightSpaceMat", dirLightSpaceMat);

	// render sceneObjects from light's perspective into shadow map
	for (const SceneObject& obj : scene.getSceneObjects()) {
		glm::mat4 modelMat{ obj.getModelMatrix() };

		const std::vector<Mesh>& meshes{ obj.getModel()->getMeshes() };
		shadowMapShader.use();
		for (const Mesh& mesh : meshes) {
			shadowMapShader.setMat4("model", obj.getModelMatrix());
			mesh.drawGeometry();
		}
	}
}

void Renderer::renderObjectForward(const SceneObject& obj, const Resources& resources) const {
	glm::mat4 modelMat{ obj.getModelMatrix() };

	for (const Mesh& mesh : obj.getModel()->getMeshes()) {
		const Material* material{getMaterial(obj, mesh)};
		const Material::Type matType{ material->getType() };

		//choose shader based on material
		const Shader* shader{ nullptr };

		// if basic material, then shader selected from material
		if (matType == Material::Basic) 
			shader = static_cast<const BasicMaterial*>(material)->shader;
		else if (matType == Material::Phong)
			shader = &resources.phongShader;
		else if (matType == Material::SolidColor)
			shader = &resources.solidColorShader;

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

		material->bind(*shader);
		mesh.drawGeometry();
	}
}

void Renderer::renderObjectDeferred(const SceneObject& obj) const {
	glm::mat4 modelMat{ obj.getModelMatrix() };

	for (const Mesh& mesh : obj.getModel()->getMeshes()) {

		//choose shader based on material
		const Material* material{ getMaterial(obj, mesh)};
		const Material::Type matType{ material->getType() };

		// only draw to gBuffer if supported material type
		if (!(matType == Material::Phong)) 
			continue;

		gBufferShader.use();

		//update model matrix
		gBufferShader.setMat4("model", modelMat);

		//update normal matrix
		glm::mat3 normalMat = glm::transpose(glm::inverse(camera.getViewMatrix() * modelMat));
		gBufferShader.setMat3("normalMat", normalMat);

		material->bind(gBufferShader);
		mesh.drawGeometry();
	}
}

void Renderer::renderObjectDeferredForward(const SceneObject& obj, const Resources& resources) const {
	glm::mat4 modelMat{ obj.getModelMatrix() };

	for (const Mesh& mesh : obj.getModel()->getMeshes()) {
		const Material* material{ getMaterial(obj, mesh) };
		const Material::Type matType{ material->getType() };

		//choose shader based on material
		const Shader* shader{ nullptr };

		// dont render if already rendered through deferred method
		if (matType == Material::Phong)
			continue;
		// if basic material, then shader selected from material
		else if (matType == Material::Basic)
			shader = static_cast<const BasicMaterial*>(material)->shader;
		else if (matType == Material::SolidColor)
			shader = &resources.solidColorShader;

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

		material->bind(*shader);
		mesh.drawGeometry();
	}
}


const Material* Renderer::getMaterial(const SceneObject& obj, const Mesh& mesh) const {
	const Material* material;
	if (obj.getMaterialOverride()) {
		material = obj.getMaterialOverride();
	}
	else {
		material = obj.getModel()->getMaterials()[mesh.getMaterialIndex()].get();
	}

	return material;
}


void Renderer::renderInstancedObject(const InstancedSceneObject& obj, const Resources& resources) const {
	const std::vector<Mesh>& meshes{ obj.model->getMeshes() };
	for (const Mesh& mesh : meshes) {
		const Material& material{ *obj.model->getMaterials()[mesh.getMaterialIndex()] };
		const Shader& shader{ resources.instanceObjectShader };

		shader.use();
		material.bind(shader);
		mesh.drawInstancedGeometry(obj.count);
	}
}

void Renderer::renderSkyBox(unsigned int texture) {
	skyboxShader.use();
	
	glEnable(GL_DEPTH_TEST);
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
	lightData.dirLight.diffuse = glm::vec4{ scene.dirLight.diffuse, 1.0f };
	lightData.dirLight.specular = glm::vec4{ scene.dirLight.specular, 1.0f };

	// point lights
	const std::vector<PointLight>& pointLights{ scene.getPointLights() };
	for (size_t i{ 0 }; i < pointLights.size(); i++) {
		const PointLight& light{ pointLights[i] };

		lightData.pointLights[i].position = view * glm::vec4{ light.obj.position, 1.0f };

		lightData.pointLights[i].attenuation = glm::vec4{ light.constant, light.linear, light.quadratic, 1.0f };

		lightData.pointLights[i].diffuse = glm::vec4{ light.diffuse, 1.0f };
		lightData.pointLights[i].specular = glm::vec4{ light.specular, 1.0f };
	}

	//spot light
	lightData.spotLight.position = glm::vec4{ 0.0f };
	lightData.spotLight.direction = glm::normalize(view * glm::vec4{ camera.front, 0.0f });
	lightData.enableFlashLightNumPtLights = { enableFlashLight, scene.getPointLights().size(), 0.0f, 0.0f };

	glBindBuffer(GL_UNIFORM_BUFFER, lightDataUBO);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(GPUData::LightData), reinterpret_cast<void*>(&lightData));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}


void Renderer::createMatricesUBO() {
	glGenBuffers(1, &matricesUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, matricesUBO);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferBase(GL_UNIFORM_BUFFER, MATRICES_UBO_BINDING, matricesUBO);
}

void Renderer::createLightDataUBO() {
	glGenBuffers(1, &lightDataUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, lightDataUBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(GPUData::LightData), NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferBase(GL_UNIFORM_BUFFER, LIGHT_DATA_UBO_BINDING, lightDataUBO);
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

	// gui setup
	gui.shadowMap = shadowMap;
}

void Renderer::setUpGBuffer(float windowWidth, float windowHeight) {
	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

	// postion color buffer
	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, windowWidth, windowHeight, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// using CLAMP_TO_EDGE for SSAO when sampling near edge of texture (maybe change to clamp to border?)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

	// normal color buffer
	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, windowWidth, windowHeight, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// using CLAMP_TO_EDGE for SSAO when sampling near edge of texture (maybe change to clamp to border?)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
	
	// color (3 bytes) + specular (1 byte) - format RGBA (8 bytes) color buffer 
	glGenTextures(1, &gAlbedoSpec);
	glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, windowWidth, windowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);

	// emission color buffer
	glGenTextures(1, &gEmission);
	glBindTexture(GL_TEXTURE_2D, gEmission);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, windowWidth, windowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gEmission, 0);

	// tell OpenGL which color attachments we'll use for rendering 
	unsigned int attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
	glDrawBuffers(4, attachments);

	// create and attach depth buffer (renderbuffer)
	unsigned int rboDepthStencil;
	glGenRenderbuffers(1, &rboDepthStencil);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepthStencil);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, windowWidth, windowHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboDepthStencil);
	
	// finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer for gBuffer is not complete!\n";

	//gui setup
	gui.gPosition = gPosition;
	gui.gNormal = gNormal;
	gui.gAlbedoSpec = gAlbedoSpec;
	gui.gEmission = gEmission;
}

void Renderer::setUpSSAO(float windowWidth, float windowHeight) {
	//create random kernal for sampling points - hemisphere in tangent space around +z axis
	std::uniform_real_distribution<float> randomFloats{ 0.0f, 1.0f }; // generates random floats between [0, 1)
	std::default_random_engine generator;
	std::vector<glm::vec3> SSAOKernel;
	constexpr int NUM_SAMPLES{ 64 };
	for (int i{ 0 }; i < NUM_SAMPLES; i++) {
		// this will generate vectors in a cube  {-1 to 1 x -1 to 1 x 0 to 1}
		glm::vec3 sample{ randomFloats(generator) * 2.0f - 1.0f,
						  randomFloats(generator) * 2.0f - 1.0f,
						  randomFloats(generator) };

		// now each vector is on surface of hemisphere instead of cube
		sample = glm::normalize(sample);

		// now each vector randomizes lenghth between 0 and 1
		sample *= randomFloats(generator);
		
		// lerp betwen 0.1 and 1.0 using scale^2 as the percentage and scale the sample by it, this gives more values towards 0.1 as compared to 1.0
		float scale{ static_cast<float>(i) / NUM_SAMPLES };
		scale = std::lerp(0.1f, 1.0f, scale * scale);
		sample *= scale;

		SSAOKernel.push_back(sample);
	}

	// set up 4x4 texture for random rotations of kernal
	std::vector<glm::vec3> SSAONoise;
	for (int i{ 0 }; i < 16; i++) {
		glm::vec3 noiseVal{ randomFloats(generator) * 2.0f - 1.0f,
							randomFloats(generator) * 2.0f - 1.0f,
							0.0f };
		SSAONoise.push_back(noiseVal);
	}

	glGenTextures(1, &SSAONoiseTexture);
	glBindTexture(GL_TEXTURE_2D, SSAONoiseTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 4, 4, 0, GL_RGB, GL_FLOAT, SSAONoise.data());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//create framebuffer for SSAO
	glGenFramebuffers(1, &SSAOFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, SSAOFBO);
	
	//only 1 color buffer attachment with just 1 component required
	glGenTextures(1, &SSAOColorBuffer);
	glBindTexture(GL_TEXTURE_2D, SSAOColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, windowWidth, windowHeight, 0, GL_RED, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, SSAOColorBuffer, 0);

	// finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer for SSAO is not complete!\n";

	// set uniforms
	SSAOShader.use();
	SSAOShader.setInt("gPosition", 0);
	SSAOShader.setInt("gNormal", 1);
	SSAOShader.setInt("noiseTexture", 2);

	// Send kernel + rotation 
	for (unsigned int i = 0; i < 64; ++i)
		SSAOShader.setVec3("samples[" + std::to_string(i) + "]", SSAOKernel[i]);

	//gui
	gui.SSAOTexture = SSAOColorBuffer;
}


void Renderer::init(float windowWidth, float windowHeight, const Resources& resources) {
	setUpPostProcessing(windowWidth, windowHeight);
	createLightDataUBO();
	createMatricesUBO();
	setUpShadowMap();
	setUpGBuffer(windowWidth, windowHeight);
	setUpSSAO(windowWidth, windowHeight);

	// set initial uniforms
	resources.deferredPhongShader.use();

	resources.deferredPhongShader.setInt("gPosition", 0);
	resources.deferredPhongShader.setInt("gNormal", 1);
	resources.deferredPhongShader.setInt("gAlbedoSpec", 2);
	resources.deferredPhongShader.setInt("gEmission", 3);
	resources.deferredPhongShader.setInt("shadowMap", SHADOW_MAP_TEXTURE_UNIT);
	resources.deferredPhongShader.setInt("SSAO", SSAO_TEXTURE_UNIT);
}
