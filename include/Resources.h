#pragma once

#include "Model.h"
#include "Mesh.h"
#include "Shader.h"
#include "Utils.h"
#include "VertexData.h"
#include "Skybox.h"

struct Resources {
	// shaders
	Shader instanceObjectShader{ "./shaders/instanceObjectVS.glsl", "./shaders/objectFS.glsl" };
	Shader objectShader{ "./shaders/objectVS.glsl", "./shaders/objectFS.glsl" };
    Shader simpleShader{ "./shaders/simpleVS.glsl", "./shaders/simpleFS.glsl" };
    Shader lightShader{ "./shaders/lightVS.glsl", "./shaders/lightFS.glsl" };
    Shader depthShader{"./shaders/depthVS.glsl", "./shaders/depthFS.glsl"};
    Shader frameBufferShader{"./shaders/frameBufferVS.glsl", "./shaders/frameBufferFS.glsl"};
    Shader skyboxShader{"./shaders/skyboxVS.glsl", "./shaders/skyboxFS.glsl"};
    Shader reflectiveShader{"./shaders/reflectiveVS.glsl", "./shaders/reflectiveFS.glsl"};
    Shader refractiveShader{"./shaders/refractiveVS.glsl", "./shaders/refractiveFS.glsl"};
    Shader explodeNormalsShader{"./shaders/explodeNormalsVS.glsl", "./shaders/explodeNormalsFS.glsl", "./shaders/explodeNormalsGS.glsl"};
    Shader normalVisShader{"./shaders/normalsVS.glsl", "./shaders/normalsFS.glsl", "./shaders/normalsGS.glsl"};
    Shader solidColorShader{ "./shaders/solidColorVS.glsl", "./shaders/solidColorFS.glsl" };

    // textures
    unsigned int container2Diff{ Utils::loadTextureFromFile("./resources/textures/container2.png") };
    unsigned int container2Spec{ Utils::loadTextureFromFile("./resources/textures/container2_specular.png") };
    unsigned int boxMarbleTex{ Utils::loadTextureFromFile("./resources/textures/marble.jpg") };
    unsigned int planeMetalTex{ Utils::loadTextureFromFile("./resources/textures/metal.png") };
    unsigned int grassTex{ Utils::loadTextureFromFile("./resources/textures/grass.png") };
    unsigned int windowTex{ Utils::loadTextureFromFile("./resources/textures/blending_transparent_window.png") };

    // meshes
    Mesh containerMesh{ VertexData::cubeNormalsTexture, {3, 3, 2}, {{container2Diff, Texture::diffuse}, {container2Spec, Texture::specular}} };
    Mesh lightMesh{ VertexData::cubeTex, {3, 2} };
    Mesh marbleCubeMesh{ VertexData::cubeNormalsTexture, {3, 3, 2}, {{boxMarbleTex, Texture::diffuse}} };
    Mesh planeMesh{ VertexData::planeTex, {3, 2}, {{planeMetalTex, Texture::diffuse}} };
    Mesh grassMesh{ VertexData::transparent, {3, 2} , {{grassTex, Texture::diffuse}} };
    Mesh windowMesh{ VertexData::transparent, {3, 2}, {{windowTex, Texture::diffuse}} };

	// models
	Model planetModel{ "./resources/models/planet/planet.obj", false };
	Model asteroidModel{ "./resources/models/rock/rock.obj", true };
    Model backpackModel{ "./resources/models/backpack/backpack.obj", false };
    Model countrySceneModel{ "./resources/models/countryside-scene-free/source/untitled.glb", true };
    Model citySceneModel{ "./resources/models/city-scene/source/Untitled.glb", false };
    Model planet2Model{ "./resources/models/planet2/pbr_dark_planet_4.glb", false };
    Model bunnyModel{ "./resources/models/bunny/stanford-bunny.obj", false };
};