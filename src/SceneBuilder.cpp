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
    scene.addObject("planet", planet);
    scene.addInstancedObject("asteroids", asteroids);

    scene.dirLight = { Colors::white };

    return scene;
}

Scene buildCityScene(Resources& r) {
    SceneObject city{ &r.citySceneModel };
    city.rotation = { -90.0f, 0.0f, 0.0f };
    //city.setMaterialOverride(r.depthMat);

    Scene scene;
    scene.dirLight = { Colors::white };
    scene.addObject("city", city);

    return scene;
}

Scene buildCountryScene(Resources& r) {
    SceneObject countrySceneObj{ &r.countrySceneModel};
    countrySceneObj.rotation = { 0.0f, 90.0f, 0.0f };

    Scene scene;
    scene.addObject("countryScene", countrySceneObj);

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

    scene.getPointLight("light1")->obj.position = glm::vec3(tilt1 * glm::vec4(p1, 1.0f)) + center;
    scene.getPointLight("light2")->obj.position = glm::vec3(tilt2 * glm::vec4(p2, 1.0f)) + center;
    scene.getPointLight("light3")->obj.position = glm::vec3(tilt3 * glm::vec4(p3, 1.0f)) + center;
}


Scene buildMainScene(Resources& r) {
    Scene scene;

    scene.dirLight = { Colors::white };

    glm::vec3 backpackPos{ 0.0f, 0.0f, 0.0f };
    SceneObject backpack{ &r.backpackModel, backpackPos };
    backpack.scale = glm::vec3{ .8f };
    scene.addObject("backpack", backpack);
    
    SceneObject backpackNormals{ backpack };
    backpackNormals.setMaterialOverride(r.normalsVisMaterial);
    //scene.addObject("backpackNormals", backpackNormals);

    SceneObject light1{ &r.lightModel };
    light1.scale = glm::vec3{ 0.2f };
    light1.setMaterialOverride(r.redMat);
    SceneObject light2{ &r.lightModel };
    light2.scale = glm::vec3{ 0.2f };
    light2.setMaterialOverride(r.greenMat);
    SceneObject light3{ &r.lightModel };
    light3.scale = glm::vec3{ 0.2f };
    light3.setMaterialOverride(r.blueMat);

    scene.addPointLight("light1", { light1, Colors::red } );
    scene.addPointLight("light2", { light2, Colors::green });
    scene.addPointLight("light3", { light3, Colors::blue });

    glm::vec3 center{ -20.0f, 0.0f, 0.0f };
    SceneObject cube1{ &r.cubeModel, center + glm::vec3{-1.0f, 0.0f, -1.0f } };
    SceneObject cube2{ &r.cubeModel, center + glm::vec3{ 2.0f, 0.0f,  0.0f } };
    scene.addObject("c1", cube1);
    scene.addObject("c2", cube2);

    SceneObject plane{ &r.planeModel , center  };
    plane.scale = glm::vec3{ 1.5f, 1.0f, 1.5f };
    scene.addObject("plane", plane);

    scene.setUpdateFunc(updateMainScene);
    return scene;
    
}

Scene buildRefScene(Resources& r) {
    Scene scene;

    glm::vec3 scale = glm::vec3{ 8.0f };
    SceneObject bunny1{ &r.bunnyModel, glm::vec3{-1.0, 0.0, 0.0}, scale };
    bunny1.setMaterialOverride(r.reflectiveMat);
    scene.addObject("bunny1", bunny1);

    SceneObject bunny2{ &r.bunnyModel, glm::vec3{1.0, 0.0, 0.0}, scale };
    bunny2.setMaterialOverride(r.refractiveMat);
    scene.addObject("bunny2", bunny2);

    return scene;
}

Scene buildLightScene(Resources& r) {
    Scene scene;

    //scene.dirLight = { Colors::white };
    SceneObject light{ &r.lightModel, {0, 1, 0}, glm::vec3{0.2f} };
    scene.addPointLight("light1", PointLight{ light, Colors::white });

    SceneObject plane{ &r.planeModel };
    scene.addObject("plane", plane);

    return scene;
}

Scene buildShadowScene(Resources& r) {
    Scene scene;
    scene.dirLight = { Colors::white };

    //SceneObject light{ &r.lightMesh, {-4, 7, 1}, glm::vec3{0.2f} };
    ////scene.addPointLight("light1", PointLight{ light, Colors::white }, &r.solidColorShader);

    //scene.dirLight = { Colors::white * 10.0f };
    for (int i{ 0 }; i < 8; i++) {
        for (int j{ 0 }; j < 8; j++) {
            PBRMaterial mat{ {0.5f, 0.0f, 0.0f}, glm::clamp(j / 7.0f, 0.05f, 1.0f), glm::clamp(i / 7.0f, 0.05f, 1.0f) };
            SceneObject sphere{ &r.sphereModel, mat };
            sphere.position = { i - 8, j + 1, 10 };
            sphere.scale = glm::vec3{ .4 };
            scene.addObject("sphere" + std::to_string(i), sphere);
        }
    }

    SceneObject s1{ &r.sphereModel, {-12.0f, 0.0f, 0.0f} };
    scene.addObject("s1", s1);

    SceneObject s2{ &r.sphereModel, r.wispyGrass };
    s2.position = { -15.0f, 0.0f, 0.0f };
    scene.addObject("s2", s2);

    SceneObject plane{ &r.planeModel };
    plane.scale = glm::vec3{ 10.0f, 1.0f, 10.0f };
    plane.setMaterialOverride(r.woodMat);
    static_cast<PhongMaterial*>(plane.getMaterialOverride())->textureScale = glm::vec2{ 10.0f };
    scene.addObject("plane", plane);

    SceneObject cube1{ &r.cubeModel, {0.0f, 1.5f, 0.0f}, glm::vec3{1.5f} };
    SceneObject cube2{ &r.cubeModel, {2.0f, 0.05f, 1.0f}, glm::vec3{1.0f} };
    SceneObject cube3{ &r.cubeModel, {-1.0f, 0.0f, 2.0}, glm::vec3{0.25f} , {132, 56, -52} };

    cube1.setMaterialOverride(r.woodMat);
    cube2.setMaterialOverride(r.woodMat);
    cube3.setMaterialOverride(r.woodMat);

    scene.addObject("cube1", cube1);
    scene.addObject("cube2", cube2);
    scene.addObject("cube3", cube3);

    return scene;
}

Scene buildNormalMapScene(Resources& r) {
    Scene scene;
    scene.dirLight = { Colors::white };

    SceneObject plane1{ &r.planeModel, r.brickWallMat };
    plane1.position = { -6, 0, 0 };
    plane1.rotation = { 90, 0, 0 };

    SceneObject planeNorms{ plane1 };
    planeNorms.setMaterialOverride(r.normalsVisMaterial);

    SceneObject plane2{ &r.planeModel, r.brickWallMat };
    plane2.position = { 6, 0, 0 };

    scene.addObject("p1", plane1);
    scene.addObject("p2", plane2);
    scene.addObject("norms", planeNorms);

    return scene;
}

Scene buildHDRScene(Resources& r) {
    Scene scene;

    //scene.dirLight = { Colors::white };

    SceneObject light1{ &r.lightModel, {-2.0f, 0.0f, 28.0f} };
    light1.scale = glm::vec3{ 0.2f };
    light1.setMaterialOverride(r.redMat);
    SceneObject light2{ &r.lightModel,  {2.0f, 0.0f, 26.0f} };
    light2.scale = glm::vec3{ 0.2f };
    light2.setMaterialOverride(r.greenMat);
    SceneObject light3{ &r.lightModel,  {0.0f, 0.0f, 24.0f} };
    light3.scale = glm::vec3{ 0.2f };
    light3.setMaterialOverride(r.blueMat);

    SceneObject light4{ &r.lightModel,  {0.0f, 0.0f, -33.0f} };
    light4.scale = glm::vec3{ 0.2f };


    scene.addPointLight("light1", { light1, Colors::red * 0.1f});
    scene.addPointLight("light2", { light2, Colors::green * 0.1f });
    scene.addPointLight("light3", { light3, Colors::blue * 0.1f });
    PointLight pl4{ light4, Colors::white * 100.0f };
    pl4.quadratic = .3;
    pl4.linear = .04;
    scene.addPointLight("light4", pl4);


    SceneObject tunnel { &r.tunnelModel };
    tunnel.scale = { 4.0f, 4.0f, 40.0f };
    PhongMaterial tunnelMat{ r.woodMat };
    tunnelMat.textureScale = glm::vec2{ 3.0f };
    tunnel.setMaterialOverride(tunnelMat);

    scene.addObject( "tunnel", tunnel );

    return scene;
}

Scene buildBackroomsScene(Resources& r) {
    Scene scene;

    //scene.dirLight = { Colors::white };
    SceneObject backrooms{ &r.backroomsModel };
    backrooms.rotation = { -90, 0, 0 };
    
    scene.addObject("backrooms", backrooms);


    return scene;
}

void updatePBRScene(Scene& scene, float deltaTime) {
    float t{ static_cast<float>(glfwGetTime())};
    constexpr float pi{ glm::pi<float>() };
    float radius{ 8.0f };

    scene.getPointLight("l1")->obj.position = glm::vec3(cos(t) * radius , sin(t) * radius, 20.0f );
    scene.getPointLight("l2")->obj.position = glm::vec3(cos(t + pi / 2) * radius, sin(t + pi / 2) * radius, 20.0f);
    scene.getPointLight("l3")->obj.position = glm::vec3(cos(t + pi ) * radius, sin(t + pi ) * radius, 20.0f);
    scene.getPointLight("l4")->obj.position = glm::vec3(cos(t + 3 * pi / 2) * radius, sin(t + 3 * pi / 2) * radius, 20.0f);
}

Scene buildPBRScene(Resources& r) {
    Scene scene;
    scene.setUpdateFunc(updatePBRScene);

    //scene.dirLight = { Colors::white * 10.0f };
    for (int i{ 0 }; i < 8; i++) {
        for (int j{ 0 }; j < 8; j++) {
            PBRMaterial mat{ {0.5f, 0.0f, 0.0f}, glm::clamp( j / 7.0f, 0.05f, 1.0f), glm::clamp(i / 7.0f, 0.05f, 1.0f) };
            SceneObject sphere{ &r.sphereModel, mat };
            sphere.position = { i * 2 - 8, j * 2 - 8, 0.0 };
            sphere.scale = glm::vec3{ .8 };
            scene.addObject("sphere" + std::to_string(i), sphere);
        }
    }

    SceneObject s1{ &r.sphereModel, {-12.0f, 0.0f, 0.0f} };
    scene.addObject("s1", s1);

    SceneObject s2{ &r.sphereModel, r.wispyGrass };
    s2.position = { -15.0f, 0.0f, 0.0f };
    scene.addObject("s2", s2);

    SceneObject pl1{ &r.lightModel,  {-8.0f, 8.0f, 20.0f} };
    pl1.scale = glm::vec3{ 0.2f };
    SceneObject pl2{ &r.lightModel,  {8.0f, -8.0f, 20.0f} };
    pl2.scale = glm::vec3{ 0.2f };
    SceneObject pl3{ &r.lightModel,  {-8.0f, -8.0f, 20.0f} };
    pl3.scale = glm::vec3{ 0.2f };
    SceneObject pl4{ &r.lightModel,  {8.0f, 8.0f, 20.0f} };
    pl4.scale = glm::vec3{ 0.2f };

    glm::vec3 lightColor{ 23.47, 21.31, 20.79 };
    //lightColor = Colors::black;
    scene.addPointLight("l1", { pl1 , lightColor });
    scene.addPointLight("l2", { pl2 , lightColor });
    scene.addPointLight("l3", { pl3 , lightColor });
    scene.addPointLight("l4", { pl4 , lightColor });

    return scene;
}

