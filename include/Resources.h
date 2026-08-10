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
	Shader instanceObjectShader{ "./shaders/instanceObjectVS.glsl", "./shaders/objectFS.glsl" };
	Shader objectShader{ "./shaders/objectVS.glsl", "./shaders/objectFS.glsl" };
    Shader simpleShader{ "./shaders/simpleVS.glsl", "./shaders/simpleFS.glsl" };
    Shader lightShader{ "./shaders/lightVS.glsl", "./shaders/lightFS.glsl" };
    Shader depthShader{"./shaders/depthVS.glsl", "./shaders/depthFS.glsl"};
    Shader reflectiveShader{"./shaders/reflectiveVS.glsl", "./shaders/reflectiveFS.glsl"};
    Shader refractiveShader{"./shaders/refractiveVS.glsl", "./shaders/refractiveFS.glsl"};
    Shader explodeNormalsShader{"./shaders/explodeNormalsVS.glsl", "./shaders/explodeNormalsFS.glsl", "./shaders/explodeNormalsGS.glsl"};
    Shader normalVisShader{"./shaders/normalsVS.glsl", "./shaders/normalsFS.glsl", "./shaders/normalsGS.glsl"};
    Shader solidColorShader{ "./shaders/solidColorVS.glsl", "./shaders/solidColorFS.glsl" };

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
    SolidColorMaterial redMat{ lightShader, Colors::red };
    SolidColorMaterial blueMat{ lightShader, Colors::blue };
    SolidColorMaterial greenMat{ lightShader, Colors::green };
    SolidColorMaterial whiteMat{ lightShader, Colors::white };
    PhongMaterial marbleMat{ objectShader, {{boxMarbleTex, Texture::diffuse}} };
    PhongMaterial woodMat{ objectShader, {{woodTex, Texture::diffuse}} };
    PhongMaterial brickWallMat{ objectShader, {{brickWallTex, Texture::diffuse}, {brickWallNormalMap, Texture::normal}}, 256.0f };
    BasicMaterial normalsVisMaterial{ normalVisShader };
    BasicMaterial reflectiveMat{ reflectiveShader };
    BasicMaterial refractiveMat{ refractiveShader };
    BasicMaterial depthMat{ depthShader };

    // meshes
    Mesh cubeMesh{ VertexData::cube, {3, 3, 2} };
    Mesh planeMesh{ VertexData::plane, {3, 3, 2}};
    Mesh tunnelMesh{ VertexData::insideOutCube, {3, 3, 2} };
    //Mesh grassMesh{ VertexData::transparent, {3, 2} , {{grassTex, Texture::diffuse}} };
    //Mesh windowMesh{ VertexData::transparent, {3, 2}, {{windowTex, Texture::diffuse}} };

	// models
    Model planetModel{ "./resources/models/planet/planet.obj", objectShader  };
    Model asteroidModel{ "./resources/models/rock/rock.obj",  instanceObjectShader, { true } };
    Model backpackModel{ "./resources/models/backpack/backpack.obj", objectShader , { false, true } };
    Model lightModel{ cubeMesh, whiteMat };
    Model cubeModel{ cubeMesh, marbleMat };
    Model planeModel{ planeMesh, woodMat };
    Model countrySceneModel{ "./resources/models/countryside-scene-free/source/untitled.glb", objectShader, { true } };
    Model citySceneModel{ "./resources/models/city-scene/source/Untitled.glb", objectShader };
    Model bunnyModel{ "./resources/models/bunny/stanford-bunny.obj", objectShader };
    Model tunnelModel{ tunnelMesh, woodMat };
};