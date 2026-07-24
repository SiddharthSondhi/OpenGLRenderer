#pragma once

#include "Shader.h"
#include "SceneObject.h"
#include "InstancedSceneObject.h"
#include "PointLight.h"
#include "Camera.h"
#include "DirectionalLight.h"

#include <vector>
#include <unordered_map>
#include <string>

extern Camera camera;

class Scene {
public:
	void addObject(std::string_view name, SceneObject obj);
	void addInstancedObject(std::string_view name, InstancedSceneObject obj);
	void addPointLight(std::string_view name, PointLight light);

	SceneObject* getObject(const std::string& name);
	InstancedSceneObject* getInstancedObject(const std::string& name);
	PointLight* getPointLight(const std::string& name);

	const std::vector<SceneObject>& getSceneObjects() const;
	const std::vector<InstancedSceneObject>& getInstancedObjects() const;
	const std::vector<PointLight>& getPointLights() const;

	using UpdateFunc = std::function<void(Scene&, float)>;
	void setUpdateFunc(UpdateFunc func);
	void update(float deltaTime);

	DirectionalLight dirLight;

private:
	std::vector<SceneObject> objects;
	std::unordered_map<std::string, size_t> objectLookupMap;

	std::vector<InstancedSceneObject> instancedObjects;
	std::unordered_map<std::string, size_t> instancedObjectLookupMap;

	std::vector<PointLight> pointLights;
	std::unordered_map<std::string, size_t> pointLightLookupMap;

	UpdateFunc updateFunc{ nullptr };
};