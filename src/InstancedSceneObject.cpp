#include "InstancedSceneObject.h"
#include "Model.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

InstancedSceneObject::InstancedSceneObject(Model* model, const std::vector<glm::mat4>& modelMatrices) 
	: model{ model },
	  count{ static_cast<int>(modelMatrices.size()) }
{
	setUpVBO(modelMatrices);

	for (const Mesh& m : model->getMeshes()) {
		// use same VBO for each mesh 
		setUpInstanceAttrib(m);
	}
}

void InstancedSceneObject::setUpVBO(const std::vector<glm::mat4>& modelMatrices) {
	glGenBuffers(1, &instanceVBO);
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glBufferData(GL_ARRAY_BUFFER, modelMatrices.size() * sizeof(glm::mat4), modelMatrices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void InstancedSceneObject::setUpInstanceAttrib(const Mesh& m) {
	glBindVertexArray(m.getVAO());
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);

	// 4 used for glm::mat4 since max size of attrib is vec4
	for (int i{ 0 }; i < 4; i++) {
		unsigned int attribIndex{ static_cast<unsigned int>(m.getAttribCount() + i) };
		glVertexAttribPointer(attribIndex, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), reinterpret_cast<void*>(sizeof(glm::vec4) * i));
		glEnableVertexAttribArray(attribIndex);
		glVertexAttribDivisor(attribIndex, 1);
	}

	glBindVertexArray(0);
}



