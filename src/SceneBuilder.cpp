#pragma once

#include "SceneBuilder.h"
#include "PointLight.h"
#include "Colors.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h> 
#include <GLFW/glfw3.h>


Scene buildPlanetScene(Resources& r) {
	SceneObject planet{ &r.planetModel , {0.0f, -3.0f, 0.0f}, glm::vec3{4.0f} };

    int amount{ 100000 };
    float radius{ 150.0f };
    float offset = { 25.0f };

    std::vector<glm::mat4> modelMats;
    for (int i{ 0 }; i < amount; i++) {
        float angle{ glm::radians(static_cast<float>(i) / amount * 360.0f) };
        float displacement{ (rand() % static_cast<int>(2 * offset * 100)) / 100.0f - offset };
        float x{ sin(angle) * radius + displacement };
        displacement = (rand() % static_cast<int>(2 * offset * 100)) / 100.0f - offset;
        float y{ displacement * 0.2f };
        displacement = (rand() % static_cast<int>(2 * offset * 100)) / 100.0f - offset;
        float z{ cos(angle) * radius + displacement };
        float scale = (rand() % 20) / 100.0f + 0.05;
        float rotAngle = (rand() % 360);

        glm::mat4 mat{ 1.0f };

        // tilt the ring
        mat = glm::rotate(mat, glm::radians(15.0f), glm::vec3(0.0f, 0.0f, 1.0f));

        mat = glm::translate(mat, { x, y, z });
        mat = glm::rotate(mat, rotAngle, glm::vec3{ 0.4f, 0.6f, 0.8f });
        mat = glm::scale(mat, glm::vec3{ scale });

        modelMats.push_back(mat);
    }

    InstancedSceneObject asteroids{ &r.asteroidModel, modelMats };
    
    Scene scene;
    scene.addObject("planet", planet, &r.objectShader);
    scene.addInstancedObject("asteroids", asteroids, &r.instanceObjectShader);

    return scene;
}

Scene buildCityScene(Resources& r) {
    SceneObject city{ &r.citySceneModel };
    city.rotation = { -90.0f, 0.0f, 0.0f };

    Scene scene;
    scene.addObject("city", city, &r.depthShader);

    return scene;
}

Scene buildCountryScene(Resources& r) {
    SceneObject countrySceneObj{ &r.countrySceneModel};
    countrySceneObj.rotation = { 0.0f, 90.0f, 0.0f };

    Scene scene;
    scene.addObject("countryScene", countrySceneObj, &r.objectShader);

    return scene;
}

void updateMainScene(Scene& scene, float deltaTime) {
    float radius{ 2.5f };
    float t{ static_cast<float>(glfwGetTime()) * 2.0f };
    constexpr float pi{ glm::pi<float>() };

    // Base orbit positions (120 degrees apart)
    glm::vec3 p1{ sin(t) * radius, 0.0f, cos(t) * radius };
    glm::vec3 p2{ sin(t + 2 * pi / 3) * radius, 0.0f, cos(t + 2 * pi / 3) * radius };
    glm::vec3 p3{ sin(t + 4 * pi / 3) * radius, 0.0f, cos(t + 4 * pi / 3) * radius };

    // Different orbital planes
    glm::mat4 tilt1 = glm::rotate(glm::mat4(1.0f), glm::radians(30.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 tilt2 = glm::rotate(glm::mat4(1.0f), glm::radians(30.0f), glm::vec3(0.0f, 0.0f, -1.0f));
    glm::mat4 tilt3 = glm::rotate(glm::mat4(1.0f), glm::radians(30.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    glm::vec3 center{ scene.getObject("backpack")->position + glm::vec3{0.0f, 1.0f, 0.0f}};

    scene.getPointLight("light1")->lightObj.position = glm::vec3(tilt1 * glm::vec4(p1, 1.0f)) + center;
    scene.getPointLight("light2")->lightObj.position = glm::vec3(tilt2 * glm::vec4(p2, 1.0f)) + center;
    scene.getPointLight("light3")->lightObj.position = glm::vec3(tilt3 * glm::vec4(p3, 1.0f)) + center;
}


Scene buildMainScene(Resources& r) {
    Scene scene;

    glm::vec3 backpackPos{ 0.0f, 0.0f, 0.0f };
    SceneObject backpack{ &r.backpackModel, backpackPos };
    backpack.scale = glm::vec3{ .8f };
    scene.addObject("backpack", backpack, &r.objectShader);
    //scene.addObject("backpack2", backpack, &r.normalVisShader);

    SceneObject light1{ &r.lightMesh };
    light1.scale = glm::vec3{ 0.2f };
    SceneObject light2{ &r.lightMesh };
    light2.scale = glm::vec3{ 0.2f };
    SceneObject light3{ &r.lightMesh };
    light3.scale = glm::vec3{ 0.2f };

    scene.addPointLight("light1", { light1, Colors::blue }, &r.lightShader);
    scene.addPointLight("light2", { light2, Colors::red }, &r.lightShader);
    scene.addPointLight("light3", { light3, Colors::green }, &r.lightShader);

    glm::vec3 center{ -20.0f, 0.0f, 0.0f };
    SceneObject marbleCube1{ &r.marbleCubeMesh, center + glm::vec3{-1.0f, 0.0f, -1.0f } };
    SceneObject marbleCube2{ &r.marbleCubeMesh, center + glm::vec3{ 2.0f, 0.0f, 0.0f } };
    scene.addObject("marbleCube1", marbleCube1, &r.reflectiveShader);
    scene.addObject("marbleCube2", marbleCube2, &r.reflectiveShader);
    scene.addObject("marbleCube3", marbleCube2, &r.normalVisShader);

    SceneObject planeMetal{ &r.planeMesh , center + glm::vec3 {0.0f, 0.0f, 0.0f} };
    planeMetal.scale = glm::vec3{ 1.5f, 1.0f, 1.5f };
    //scene.addObject("plane", planeMetal, &r.reflectiveShader);

    std::vector<glm::vec3> vegetationPos{
        center + glm::vec3(-2.5f, 0.0f, 2.48f),
        center + glm::vec3(1.5f, 0.0f, 3.51f),
        center + glm::vec3(0.0f, 0.0f, 2.03f),
        center + glm::vec3(-0.3f, 0.0f, 1.3f),
        center + glm::vec3(0.5f, 0.0f, 0.6f)
    };

    std::vector<SceneObject> vegetation{};
    for (auto pos : vegetationPos) {
        vegetation.push_back(SceneObject{ &r.grassMesh, pos });
    }

    for (int i{ 0 }; i < vegetation.size(); i++) {
        scene.addObject("v" + std::to_string(i), vegetation[i], &r.simpleShader);
    }

    scene.setUpdateFunc(updateMainScene);
    return scene;
    
    //std::vector <glm::vec3> windowsPos{
    //    glm::vec3(-2.5f, 0.0f, -29.48f),
    //    glm::vec3(-.5f, 0.0f, -28.51f),
    //    glm::vec3(1.6f, 0.0f, -25.51f),
    //};

    //std::vector<SceneObject> windows{};
    //for (auto pos : windowsPos) {
    //    windows.push_back(SceneObject{ &windowMesh, pos });
    //}
}

Scene buildRefScene(Resources& r) {
    Scene scene;

    glm::vec3 scale = glm::vec3{ 8.0f };
    scene.addObject("bunny", { &r.bunnyModel, glm::vec3{-1.0, 0.0, 0.0}, scale }, &r.reflectiveShader);
    scene.addObject("bunny", { &r.bunnyModel, glm::vec3{ 1.0, 0.0, 0.0}, scale }, &r.refractiveShader);


    return scene;
}