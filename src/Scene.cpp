#include "Scene.h"

#include <iostream>

void Scene::draw() const {
	// draw lights
	for (const PointLightRenderItem& l : pointLights) {
		l.light.draw(*(l.shader));
	}

	// draw objects
	for (const RenderItem& r : objects) {
		r.obj.draw(*(r.shader));
	}

	// draw instanced objects
	for (const InstancedRenderItem& r : instancedObjects) {
		r.obj.draw(*(r.shader));
	}
}

void Scene::addObject(std::string_view name, SceneObject obj, Shader* shader) {
	// add to unordered map for O(1) lookup
	objectLookupMap[std::string{ name }] = objects.size();

	// add to object vector for rendering
	objects.push_back({obj, shader});
}

SceneObject* Scene::getObject(const std::string& name) {
	auto search{ objectLookupMap.find(name) };

	if (search == objectLookupMap.end()) {
		std::cout << "SCENE_OBJECT::" << name << "::NOT_FOUND\n";
		return nullptr;
	}

	return &objects[search->second].obj;
}

void Scene::addInstancedObject(std::string_view name, InstancedSceneObject obj, Shader* shader) {
	// add to unordered map for O(1) lookup
	instancedObjectLookupMap[std::string{ name }] = instancedObjects.size();

	// add to object vector for rendering
	instancedObjects.push_back({ obj, shader });
}

InstancedSceneObject* Scene::getInstancedObject(const std::string& name) {
	auto search{ instancedObjectLookupMap.find(name) };

	if (search == instancedObjectLookupMap.end()) {
		std::cout << "INSTANCED_SCENE_OBJECT::" << name << "::NOT_FOUND\n";
		return nullptr;
	}

	return &instancedObjects[search->second].obj;
}

void Scene::addPointLight(std::string_view name, PointLight light, Shader* shader) {
	if (pointLights.size() >= GPUData::MAX_NUMBER_POINT_LIGHTS) {
		std::cout << "EXCEEDED MAX NUM POINT LIGHTS IN SCENE\n";
		return;
	}

	// add to unordered map for O(1) lookup
	pointLightLookupMap[std::string{ name }] = pointLights.size();

	// add to object vector for rendering
	pointLights.push_back({ light, shader });
}

PointLight* Scene::getPointLight(const std::string& name) {
	auto search{ pointLightLookupMap.find(name) };

	if (search == pointLightLookupMap.end()) {
		std::cout << "POINT_LIGHT::" << name << "::NOT_FOUND\n";
		return nullptr;
	}

	return &pointLights[search->second].light;
}

void Scene::update(float deltaTime) {
	if (updateFunc)
		updateFunc(*this, deltaTime);

	updateLightData();
}

void Scene::updateLightData() {
	glm::mat4 view{ camera.getViewMatrix() };

	//directional light
	lightData.dirLight.direction = view * glm::vec4{ -0.3f, -1.0f, -0.2f , 0.0f };

	// point lights
	for (size_t i{ 0 }; i < pointLights.size(); i++) {
		const PointLight& light{ pointLights[i].light };

		lightData.pointLights[i].position =  view * glm::vec4{ light.lightObj.position, 1.0f };
		lightData.pointLights[i].diffuse = glm::vec4{ light.color, 1.0f };
		lightData.pointLights[i].specular = glm::vec4{ light.color, 1.0f };
	}

	//spot light
	lightData.spotLight.position = glm::vec4{ 0.0f };
	lightData.spotLight.direction = view * glm::vec4{camera.front, 0.0f };
}

void Scene::setUpdateFunc(UpdateFunc func) {
	updateFunc = func;
}

