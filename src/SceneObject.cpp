#include "SceneObject.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


SceneObject::SceneObject(Model* model, glm::vec3 postion, glm::vec3 scale, glm::vec3 rotation)
	: model{ model },
	position{ postion },
	scale{ scale },
	rotation{ rotation } {}

SceneObject::SceneObject(Model* model, const Material& materialOverride)
	: model{ model },
	materialOverride {materialOverride.clone()} {}

SceneObject::SceneObject(const SceneObject& other) 
	: model { other.model },
	position{ other.position },
	scale{ other.scale },
	rotation{ other.rotation },
	materialOverride {other.materialOverride ? other.materialOverride->clone() : nullptr}
{}

SceneObject& SceneObject::operator=(const SceneObject& other) {
	if (this == &other) {
		return *this;
	}

	model = other.model;
	position = other.position;
	scale = other.scale;
	rotation = other.rotation;
	materialOverride = other.materialOverride ? other.materialOverride->clone() : nullptr;

	return *this;
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

void SceneObject::setMaterialOverride(const Material& material) {
	materialOverride = material.clone();
}

void SceneObject::resetMaterialOverride() {
	materialOverride.reset();
}

Material* SceneObject::getMaterialOverride() const {
	return materialOverride.get();
}

const Model* SceneObject::getModel() const {
	return model;
}

