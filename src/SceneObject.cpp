#include "SceneObject.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>


SceneObject::SceneObject(Model* model, glm::vec3 postion, glm::vec3 scale, glm::vec3 rotation)
	: model{ model },
	position{ postion },
	scale{ scale },
	rotation{ rotation } {}

SceneObject::SceneObject(Mesh* mesh, glm::vec3 postion, glm::vec3 scale, glm::vec3 rotation)
	: mesh{ mesh },
	position{ postion },
	scale{ scale },
	rotation{ rotation } {}

void SceneObject::draw(const Shader& shader) const {
	shader.use();

	//update model matrix
	glm::mat4 model{ getModelMatrix() };
	shader.setMat4("model", model);

	//update normal matrix
	glm::mat3 normalMat = glm::transpose(glm::inverse(camera.getViewMatrix() * model));
	shader.setMat3("normalMat", normalMat);

	if (this->model) {
		this->model->draw(shader);
	}
	else if (mesh) {
		mesh->draw(shader);
	}
	else {
		std::cout << "SCENE_OBJECT::NO VALID MESH OR MODEL" << "\n";
	}
}

glm::mat4 SceneObject::getModelMatrix() const {
	glm::mat4 model{ 1.0f };

	//apply translation
	model = glm::translate(model, position) ;

	//apply rotations
	model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1, 0, 0));
	model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0, 1, 0));
	model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0, 0, 1));

	//apply scale
	model = glm::scale(model, scale);

	return model;
}