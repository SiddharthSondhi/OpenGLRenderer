#pragma once

#include "Model.h"
#include "Mesh.h"
#include "Shader.h"
#include "Utils.h"
#include "VertexData.h"
#include "Colors.h"
#include "SolidColorMaterial.h"
#include "PhongMaterial.h"
#include "BasicMaterial.h"
#include "PBRMaterial.h"

#include <array>

struct Resources {
    // shaders
    Shader instanceObjectShader{ "./shaders/instanceObjectVS.glsl", "./shaders/phongFS.glsl" };
    Shader phongShader{ "./shaders/phongVS.glsl", "./shaders/phongFS.glsl" };
    Shader solidColorShader{ "./shaders/solidColorVS.glsl", "./shaders/solidColorFS.glsl" };
    Shader depthShader{ "./shaders/depthVS.glsl", "./shaders/depthFS.glsl" };
    Shader reflectiveShader{ "./shaders/reflectiveVS.glsl", "./shaders/reflectiveFS.glsl" };
    Shader refractiveShader{ "./shaders/refractiveVS.glsl", "./shaders/refractiveFS.glsl" };
    Shader explodeNormalsShader{ "./shaders/explodeNormalsVS.glsl", "./shaders/explodeNormalsFS.glsl", "./shaders/explodeNormalsGS.glsl" };
    Shader normalVisShader{ "./shaders/normalsVS.glsl", "./shaders/normalsFS.glsl", "./shaders/normalsGS.glsl" };
    Shader deferredPhongShader{ "./shaders/postProcVS.glsl", "./shaders/deferredLightingFS.glsl" };

    //skyboxes
    std::array<unsigned int, 9> skyboxes{
        0,
        Utils::loadCubemap("./resources/textures/cubemaps/SkyHighFluffyCloud"),
        Utils::loadCubemap("./resources/textures/cubemaps/PlanetaryEarth"),
        Utils::loadCubemap("./resources/textures/cubemaps/CasualDay"),
        Utils::loadCubemap("./resources/textures/cubemaps/DayInTheClouds"),
        Utils::loadCubemap("./resources/textures/cubemaps/DarkStorm"),
        Utils::loadCubemap("./resources/textures/cubemaps/space1"),
        Utils::loadCubemap("./resources/textures/cubemaps/space2"),
        Utils::loadCubemap("./resources/textures/cubemaps/space3")
    };

    // textures
    unsigned int container2Diff{ Utils::loadTextureFromFile("./resources/textures/container2.png", true) };
    unsigned int container2Spec{ Utils::loadTextureFromFile("./resources/textures/container2_specular.png") };
    unsigned int boxMarbleTex{ Utils::loadTextureFromFile("./resources/textures/marble.jpg", true) };
    unsigned int planeMetalTex{ Utils::loadTextureFromFile("./resources/textures/metal.png", true) };
    unsigned int grassTex{ Utils::loadTextureFromFile("./resources/textures/grass.png") };
    unsigned int windowTex{ Utils::loadTextureFromFile("./resources/textures/blending_transparent_window.png") };
    unsigned int woodTex{ Utils::loadTextureFromFile("./resources/textures/wood.jpg", true) };
    unsigned int brickWallTex{ Utils::loadTextureFromFile("./resources/textures/brickwall.jpg", true) };
    unsigned int brickWallNormalMap{ Utils::loadTextureFromFile("./resources/textures/brickwall_normal.jpg") };

    unsigned int PBR1Albedo{ Utils::loadTextureFromFile("./resources/textures/rustediron/rustediron2_basecolor.png", true) };
    unsigned int PBR1Metallic{ Utils::loadTextureFromFile("./resources/textures/rustediron/rustediron2_metallic.png") };
    unsigned int PBR1Normal{ Utils::loadTextureFromFile("./resources/textures/rustediron/rustediron2_normal.png") };
    unsigned int PBR1roughness{ Utils::loadTextureFromFile("./resources/textures/rustediron/rustediron2_roughness.png") };

    unsigned int PBR2Albedo{ Utils::loadTextureFromFile("./resources/textures/wispy-grass-meadow/wispy-grass-meadow_albedo.png", true) };
    unsigned int PBR2AO{ Utils::loadTextureFromFile("./resources/textures/wispy-grass-meadow/wispy-grass-meadow_ao.png") };
    unsigned int PBR2Normal{ Utils::loadTextureFromFile("./resources/textures/wispy-grass-meadow/wispy-grass-meadow_normal-ogl.png") };
    unsigned int PBR2Roughness{ Utils::loadTextureFromFile("./resources/textures/wispy-grass-meadow/wispy-grass-meadow_roughness.png") };
    unsigned int PBR2Metallic{ Utils::loadTextureFromFile("./resources/textures/wispy-grass-meadow/wispy-grass-meadow_metallic.png") };


    // materials
    SolidColorMaterial redMat{ Colors::red };
    SolidColorMaterial blueMat{ Colors::blue };
    SolidColorMaterial greenMat{ Colors::green };
    SolidColorMaterial whiteMat{ Colors::white };
    PhongMaterial marbleMat{ boxMarbleTex };
    PhongMaterial woodMat{ woodTex };
    PhongMaterial brickWallMat{ brickWallTex, 0, brickWallNormalMap };
    BasicMaterial normalsVisMaterial{ normalVisShader };
    BasicMaterial reflectiveMat{ reflectiveShader };
    BasicMaterial refractiveMat{ refractiveShader };
    BasicMaterial depthMat{ depthShader };
    PBRMaterial pbrMat{ {0.5, 0.0, 0.0}, .8, .35 };
    PBRMaterial rustedIronMat{ PBR1Albedo, PBR1Metallic, PBR1roughness, 0, PBR1Normal };
    PBRMaterial wispyGrass{ PBR2Albedo, PBR2Metallic, PBR2Roughness, PBR2AO, PBR2Normal };

    // meshes
    Mesh cubeMesh{ VertexData::cube, {3, 3, 2} };
    Mesh planeMesh{ VertexData::plane, {3, 3, 2} };
    Mesh tunnelMesh{ VertexData::insideOutCube, {3, 3, 2} };
    Mesh sphereMesh{ VertexData::genSphereVertices(), {3, 3, 2, 3, 3}, 0, VertexData::genSphereIndices() };

    // models
    Model planetModel{ "./resources/models/planet/planet.obj" };
    Model asteroidModel{ "./resources/models/rock/rock.obj", { true } };
    Model backpackModel{ "./resources/models/backpack/backpack.obj", { false, true } };
    Model lightModel{ cubeMesh, whiteMat };
    Model cubeModel{ cubeMesh, marbleMat };
    Model planeModel{ planeMesh, woodMat };
    Model countrySceneModel{ "./resources/models/countryside-scene-free/source/untitled.glb", { true } };
    Model citySceneModel{ "./resources/models/city-scene/source/Untitled.glb" , { true } };
    Model bunnyModel{ "./resources/models/bunny/stanford-bunny.obj" };
    Model tunnelModel{ tunnelMesh, woodMat };
    Model backroomsModel{ "./resources/models/backrooms/backrooms_vr.glb", {true, true, false} };
    Model sphereModel{ sphereMesh, rustedIronMat };
    Model gunModel{ "./resources/models/Cerberus_by_Andrew_Maximov/fixed_model.glb", {true, true, true, true}, Material::PBR };
};