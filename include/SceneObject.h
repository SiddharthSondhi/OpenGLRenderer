#pragma once

#include <glad/glad.h>

#include "Shader.h"
#include "Model.h"
#include "Camera.h"

extern Camera camera;

class SceneObject {
public:
	glm::vec3 position{};
	glm::vec3 rotation{};
	glm::vec3 scale{};

	Model* model{ nullptr };
	Mesh* mesh{ nullptr };

	SceneObject(Model* model, glm::vec3 postion = { 0.0f, 0.0f, 0.0f }, glm::vec3 scale = { 1.0f, 1.0f, 1.0f }, glm::vec3 rotation = { 0.0f, 0.0f, 0.0f });
	SceneObject(Mesh* mesh, glm::vec3 postion = { 0.0f, 0.0f, 0.0f }, glm::vec3 scale = { 1.0f, 1.0f, 1.0f }, glm::vec3 rotation = { 0.0f, 0.0f, 0.0f });

	void draw(const Shader& shader) const;


private:
	glm::mat4 getModelMatrix() const;

};