#pragma once

#include "Shader.h"
#include "SceneObject.h"
#include "InstancedSceneObject.h"
#include "PointLight.h"
#include "LightData.h"
#include "Camera.h"

#include <vector>
#include <unordered_map>
#include <string>

extern Camera camera;

struct RenderItem{
	SceneObject obj;
	Shader* shader;
};

struct InstancedRenderItem {
	InstancedSceneObject obj;
	Shader* shader;
};

struct PointLightRenderItem {
	PointLight light;
	Shader* shader;
};


class Scene {
public:
	using UpdateFunc = std::function<void(Scene&, float)>;

	void addObject(std::string_view name, SceneObject obj, Shader* shader);
	SceneObject* getObject(const std::string& name);

	void addInstancedObject(std::string_view name, InstancedSceneObject obj, Shader* shader);
	InstancedSceneObject* getInstancedObject(const std::string& name);

	void addPointLight(std::string_view name, PointLight light, Shader* shader);
	PointLight* getPointLight(const std::string& name);

	void setUpdateFunc(UpdateFunc func);
	void update(float deltaTime);

	void draw() const;

	GPUData::LightData lightData;

private:
	std::vector<RenderItem> objects;
	std::unordered_map<std::string, size_t> objectLookupMap;

	std::vector<InstancedRenderItem> instancedObjects;
	std::unordered_map<std::string, size_t> instancedObjectLookupMap;

	std::vector<PointLightRenderItem> pointLights;
	std::unordered_map<std::string, size_t> pointLightLookupMap;

	UpdateFunc updateFunc{ nullptr };

	void updateLightData();
};