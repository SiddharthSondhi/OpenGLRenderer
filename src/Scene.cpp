#include "Scene.h"
#include "LightData.h"

#include <iostream>


void Scene::addObject(std::string_view name, SceneObject obj) {
	// add to unordered map for O(1) lookup
	objectLookupMap[std::string{ name }] = objects.size();

	// add to object vector for rendering
	objects.push_back(obj);
}

void Scene::addInstancedObject(std::string_view name, InstancedSceneObject obj) {
	// add to unordered map for O(1) lookup
	instancedObjectLookupMap[std::string{ name }] = instancedObjects.size();

	// add to object vector for rendering
	instancedObjects.push_back(obj);
}

void Scene::addPointLight(std::string_view name, PointLight light) {
	if (pointLights.size() >= GPUData::MAX_NUMBER_POINT_LIGHTS) {
		std::cout << "EXCEEDED MAX NUM POINT LIGHTS IN SCENE\n";
		return;
	}

	// add to unordered map for O(1) lookup
	pointLightLookupMap[std::string{ name }] = pointLights.size();

	// add to object vector for rendering
	pointLights.push_back(light);
}

SceneObject* Scene::getObject(const std::string& name) {
	auto search{ objectLookupMap.find(name) };

	if (search == objectLookupMap.end()) {
		std::cout << "SCENE_OBJECT::" << name << "::NOT_FOUND\n";
		return nullptr;
	}

	return &objects[search->second];
}

InstancedSceneObject* Scene::getInstancedObject(const std::string& name) {
	auto search{ instancedObjectLookupMap.find(name) };

	if (search == instancedObjectLookupMap.end()) {
		std::cout << "INSTANCED_SCENE_OBJECT::" << name << "::NOT_FOUND\n";
		return nullptr;
	}

	return &instancedObjects[search->second];
}

PointLight* Scene::getPointLight(const std::string& name) {
	auto search{ pointLightLookupMap.find(name) };

	if (search == pointLightLookupMap.end()) {
		std::cout << "POINT_LIGHT::" << name << "::NOT_FOUND\n";
		return nullptr;
	}

	return &pointLights[search->second];
}

void Scene::update(float deltaTime) {
	if (updateFunc)
		updateFunc(*this, deltaTime);
}

void Scene::setUpdateFunc(UpdateFunc func) {
	updateFunc = func;
}

const std::vector<SceneObject>& Scene::getSceneObjects() const {
	return objects;
}

const std::vector<PointLight>& Scene::getPointLights() const {
	return pointLights;
}

const std::vector<InstancedSceneObject>& Scene::getInstancedObjects() const {
	return instancedObjects;
}


