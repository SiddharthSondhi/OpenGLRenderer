#pragma once

#include <glad/glad.h>

#include "Shader.h"
#include "Model.h"
#include "Mesh.h"

#include <glm/glm.hpp>

#include <vector>


class InstancedSceneObject {
public:
	Model* model{ nullptr };
	int count;

	InstancedSceneObject(Model* model, const std::vector<glm::mat4>& modelMatrices);

private:
	unsigned int instanceVBO{ 0 };

	void setUpVBO(const std::vector<glm::mat4>& modelMatrices);
	void setUpInstanceAttrib(const Mesh& m);
};