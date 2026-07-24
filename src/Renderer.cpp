#include "Renderer.h"
#include "LightData.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

void Renderer::render(const Scene& scene, Resources& resources) {
	//before rendering bind to the framebuffer if it has been set up
	if (framebuffer) {
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		frameBufferShader.use();
		frameBufferShader.setFloat("offset", 1.0f / gui.convMatrixOffset);
		frameBufferShader.setInt("postProcessingMode", gui.postProcessingMode);
	}
	else
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);


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

	if (framebuffer) {
		// now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.

		// clear all relevant buffers
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// draw screen quad
		frameBufferShader.use();
		glActiveTexture(0);
		frameBufferShader.setInt("screenTexture", 0);
		glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
		screenQuadMesh.drawGeometry();
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
	lightData.dirLight.direction = view * glm::vec4{ scene.dirLight.direction, 0.0f };
	lightData.dirLight.ambient = glm::vec4{ scene.dirLight.ambient, 1.0f };
	lightData.dirLight.diffuse = glm::vec4{ scene.dirLight.diffuse, 1.0f };
	lightData.dirLight.specular = glm::vec4{ scene.dirLight.specular, 1.0f };

	// point lights
	const std::vector<PointLight>& pointLights{ scene.getPointLights() };
	for (size_t i{ 0 }; i < pointLights.size(); i++) {
		const PointLight& light{ pointLights[i] };

		lightData.pointLights[i].position = view * glm::vec4{ light.obj.position, 1.0f };
		lightData.pointLights[i].diffuse = glm::vec4{ light.color, 1.0f };
		lightData.pointLights[i].specular = glm::vec4{ light.color, 1.0f };
	}

	//spot light
	lightData.spotLight.position = glm::vec4{ 0.0f };
	lightData.spotLight.direction = view * glm::vec4{ camera.front, 0.0f };

	lightData.enableFlashLight = enableFlashLight ? glm::vec4(1.0f) : glm::vec4(0.0f);

	this->lightData = lightData;
}

void Renderer::uploadLightData() {
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
	//framebuffer
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	// generate texture
	glGenTextures(1, &textureColorbuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, windowWidth, windowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	// attach it to currently bound framebuffer object
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);

	//renderbuffer for depth/stencil testing
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, windowWidth, windowHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// attach it to currently bound framebuffer object
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::updateAndUploadMatrices(float windowWidth, float windowHeight) const {
	// calculate view and projection matrix
	glm::mat4 view{ camera.getViewMatrix() };
	glm::mat4 projection{ glm::perspective(glm::radians(camera.zoom), static_cast<float>(windowWidth) / windowHeight, 0.1f, 500.0f) };

	glBindBuffer(GL_UNIFORM_BUFFER, matricesUBO);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(view));
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projection));
}
