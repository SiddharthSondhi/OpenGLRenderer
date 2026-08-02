#pragma once

#include <glad/glad.h>

#include "Shader.h"
#include "Model.h"
#include "Camera.h"
#include "Material.h"

#include <memory>

extern Camera camera;

class SceneObject {
public:
	glm::vec3 position{ 0.0f };
	glm::vec3 rotation{ 0.0f };
	glm::vec3 scale{ 1.0f };

	SceneObject(Model* model, glm::vec3 postion = { 0.0f, 0.0f, 0.0f }, glm::vec3 scale = { 1.0f, 1.0f, 1.0f }, glm::vec3 rotation = { 0.0f, 0.0f, 0.0f });
	SceneObject(Model* model, const Material& materialOverride);
	
	SceneObject(const SceneObject& other);
	SceneObject& operator=(const SceneObject& other);

	SceneObject(SceneObject&& other) noexcept = default;
	SceneObject& operator=(SceneObject&& other) noexcept = default;

	glm::mat4 getModelMatrix() const;
	const Model* getModel() const;

	Material* getMaterialOverride() const;
	void setMaterialOverride(const Material& material);
	void resetMaterialOverride();

private:
	const Model* model{ nullptr };
	std::unique_ptr<Material> materialOverride{ nullptr };
};