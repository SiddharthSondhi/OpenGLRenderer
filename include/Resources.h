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

#include <array>

struct Resources {
	// shaders
	Shader instanceObjectShader{ "./shaders/instanceObjectVS.glsl", "./shaders/phongFS.glsl" };
	Shader phongShader{ "./shaders/phongVS.glsl", "./shaders/phongFS.glsl" };
    Shader solidColorShader{ "./shaders/solidColorVS.glsl", "./shaders/solidColorFS.glsl" };
    Shader depthShader{"./shaders/depthVS.glsl", "./shaders/depthFS.glsl"};
    Shader reflectiveShader{"./shaders/reflectiveVS.glsl", "./shaders/reflectiveFS.glsl"};
    Shader refractiveShader{"./shaders/refractiveVS.glsl", "./shaders/refractiveFS.glsl"};
    Shader explodeNormalsShader{"./shaders/explodeNormalsVS.glsl", "./shaders/explodeNormalsFS.glsl", "./shaders/explodeNormalsGS.glsl"};
    Shader normalVisShader{"./shaders/normalsVS.glsl", "./shaders/normalsFS.glsl", "./shaders/normalsGS.glsl"};
    Shader deferredPhongShader{ "./shaders/frameBufferVS.glsl", "./shaders/deferredPhongFS.glsl" };

    //skyboxes
    std::array<unsigned int, 13> skyboxes{
        0,
        Utils::loadCubemap("./resources/textures/cubemaps/SkyHighFluffyCloud"),
        Utils::loadCubemap("./resources/textures/cubemaps/PlanetaryEarth"),
        Utils::loadCubemap("./resources/textures/cubemaps/MegaSun"),
        Utils::loadCubemap("./resources/textures/cubemaps/highFantasy"),
        Utils::loadCubemap("./resources/textures/cubemaps/underTheSea"),
        Utils::loadCubemap("./resources/textures/cubemaps/CasualDay"),
        Utils::loadCubemap("./resources/textures/cubemaps/DayInTheClouds"),
        Utils::loadCubemap("./resources/textures/cubemaps/DarkStorm"),
        Utils::loadCubemap("./resources/textures/cubemaps/CoriolisNight"),
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


    // materials
    SolidColorMaterial redMat{ Colors::red };
    SolidColorMaterial blueMat{ Colors::blue };
    SolidColorMaterial greenMat{ Colors::green };
    SolidColorMaterial whiteMat{ Colors::white };
    PhongMaterial marbleMat{ boxMarbleTex };
    PhongMaterial woodMat{ woodTex };
    PhongMaterial brickWallMat{ brickWallTex, 0, brickWallNormalMap, 256.0f };
    BasicMaterial normalsVisMaterial{ normalVisShader };
    BasicMaterial reflectiveMat{ reflectiveShader };
    BasicMaterial refractiveMat{ refractiveShader };
    BasicMaterial depthMat{ depthShader };

    // meshes
    Mesh cubeMesh{ VertexData::cube, {3, 3, 2} };
    Mesh planeMesh{ VertexData::plane, {3, 3, 2}};
    Mesh tunnelMesh{ VertexData::insideOutCube, {3, 3, 2} };

	// models
    Model planetModel{ "./resources/models/planet/planet.obj" };
    Model asteroidModel{ "./resources/models/rock/rock.obj", { true } };
    Model backpackModel{ "./resources/models/backpack/backpack.obj", { false, true } };
    Model lightModel{ cubeMesh, whiteMat };
    Model cubeModel{ cubeMesh, marbleMat };
    Model planeModel{ planeMesh, woodMat };
    Model countrySceneModel{ "./resources/models/countryside-scene-free/source/untitled.glb", { true } };
    Model citySceneModel{ "./resources/models/city-scene/source/Untitled.glb" };
    Model bunnyModel{ "./resources/models/bunny/stanford-bunny.obj" };
    Model tunnelModel{ tunnelMesh, woodMat };
};