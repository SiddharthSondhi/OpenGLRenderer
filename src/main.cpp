#include "Shader.h"
#include "Camera.h"
#include "Mesh.h"
#include "Utils.h"
#include "VertexData.h"
#include "SceneObject.h"
#include "GUI.h"
#include "Skybox.h"
#include "Scene.h"
#include "Resources.h"
#include "SceneBuilder.h"
#include "LightData.h"

#include <glad/glad.h> 
#include <GLFW/glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/norm.hpp>

#include <iostream>
#include <array>
#include <cmath>


//prototypes
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

void orbitLights(SceneObject& light1, SceneObject& light2, SceneObject& light3);


//global variables
int windowWidth;
int windowHeight;

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

// position of mouse last frame, used to calculate yaw and pitch for camera when moving mouse
float lastMousePosX{ static_cast<float>(windowWidth) / 2 };
float lastMousePosY{ static_cast<float>(windowHeight) / 2 };  
bool firstMouse{ true };
bool mouseGUIEnabled{ false };

Camera camera{ glm::vec3{0.0f, 0.0f, 15.0f } };

bool enableFlashLight{ false };
static glm::vec3 backpackPos{ 31.0f, 3.0f, 0.0f };


int main() {
    // ---------------------------------------------SETUP---------------------------------------------
    //glfw
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // FOR MAC OS
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    windowWidth = mode->width;
    windowHeight = mode->height;

    glfwWindowHint(GLFW_SAMPLES, 4);
    GLFWwindow* window{ glfwCreateWindow(windowWidth, windowHeight, "OpenGL Window", NULL, NULL) };
    if (window == NULL) {
        std::cout << "Failed to create GLFW window.\n";
        glfwTerminate();
        return -1;
    }
    glfwSetWindowPos(window, 0, 0);

    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSwapInterval(0); // 0 = V-Sync off, 1 = V-Sync on

    //glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD.\n";
        return -1;
    }

    //setup callbacks
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);
    glfwSetKeyCallback(window, keyCallback);

    // initialize gui
    GUI::init(window);
    GUI::Settings gui;


    //framebuffer
    GLuint framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // generate texture
    GLuint textureColorbuffer;
    glGenTextures(1, &textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, windowWidth, windowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    // attach it to currently bound framebuffer object
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);

    //renderbuffer for depth/stencil testing
    GLuint rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, windowWidth, windowHeight);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //matrices UBO
    unsigned int uboMatrices;
    glGenBuffers(1, &uboMatrices);
    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, uboMatrices);
    
    //light UBO
    unsigned int uboLightData;
    glGenBuffers(1, &uboLightData);
    glBindBuffer(GL_UNIFORM_BUFFER, uboLightData);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(GPUData::LightData), NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, uboLightData);

    std::array<unsigned int, 13> skyboxes  {
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

    // meshes and models
    Mesh screenQuadMesh{ VertexData::screenQuad, {2, 2}, {{textureColorbuffer, Texture::diffuse}} };

    Skybox skybox{ VertexData::skyboxVertices, 0};

    // scene objects
    Resources resources;
    Scene mainScene{ buildMainScene(resources) };
    Scene planetScene{ buildPlanetScene(resources) };
    Scene cityScene{ buildCityScene(resources) };
    Scene countryScene{ buildCountryScene(resources) };
    Scene refScene{ buildRefScene(resources) };

    std::array<Scene*, 5> scenes{ &mainScene, &planetScene, &cityScene, &countryScene, &refScene };

    // ----------------------------------------Uniforms----------------------------------------------
    resources.objectShader.use();
    resources.objectShader.setFloat("material.shininess", 256.0f);

    resources.instanceObjectShader.use();
    resources.instanceObjectShader.setFloat("material.shininess", 256.0f);

    // settings
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_STENCIL_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);
    glEnable(GL_MULTISAMPLE);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //wireframe mode

    // ---------------------------------------Rendering Loop-------------------------------------------
    while (!glfwWindowShouldClose(window)) {
        // time logic
        float currentFrame{ static_cast<float>(glfwGetTime()) };
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input processing
        glfwPollEvents();
        processInput(window);

        //before rendering bind to the framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glEnable(GL_DEPTH_TEST);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        // calculate view and projection matrix
        glm::mat4 view{ camera.getViewMatrix() };
        glm::mat4 projection{ glm::perspective(glm::radians(camera.zoom), static_cast<float>(windowWidth) / windowHeight, 0.1f, 500.0f) };

        glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(view));
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projection));

        //update scene
        Scene* currentScene{ scenes[gui.currentSceneIndex] };
        currentScene->update(deltaTime);

        //upload light UBO
        mainScene.lightData.enableFlashLight = enableFlashLight ? glm::vec4(1.0f) : glm::vec4(0.0f);
        planetScene.lightData.enableFlashLight = enableFlashLight ? glm::vec4(1.0f) : glm::vec4(0.0f);
        GPUData::uploadLightData(currentScene->lightData, uboLightData);

        // post processing 
        resources.frameBufferShader.use();
        resources.frameBufferShader.setFloat("offset", 1.0f / gui.convMatrixOffset);
        resources.frameBufferShader.setInt("postProcessingMode", gui.postProcessingMode);

        //explode normals
        resources.explodeNormalsShader.use();
        resources.explodeNormalsShader.setFloat("time", static_cast<float>(glfwGetTime()));
        
        currentScene->draw();
        
        // semi transparent
        //std::sort(windows.begin(), windows.end(),
        //    [](const auto& a, const auto& b) {
        //        return glm::length2(camera.position - a.position) > glm::length2(camera.position - b.position);
        //    }
        //);

        //for (auto& w : windows) {
        //    //w.draw(simpleShader);
        //}

        //skybox
        skybox.texture = skyboxes[gui.skyboxIndex];
        skybox.draw(resources.skyboxShader);

        // now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
        
        // clear all relevant buffers
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // draw screen quad (postprocessing)
        screenQuadMesh.draw(resources.frameBufferShader);

        // gui
        GUI::define(gui);
        GUI::render();

        glfwSwapBuffers(window);
    }
    // cleanup
    GUI::shutDown();
    glfwTerminate();
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    //camera movement
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.processKeyboard(Camera::Camera_Movement::FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.processKeyboard(Camera::Camera_Movement::BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.processKeyboard(Camera::Camera_Movement::LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.processKeyboard(Camera::Camera_Movement::RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.processKeyboard(Camera::Camera_Movement::UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.processKeyboard(Camera::Camera_Movement::DOWN, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) 
        camera.movementSpeed = 30.0f;
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_RELEASE)
        camera.movementSpeed = 10.0f;
}

void mouseCallback(GLFWwindow* window, double xposIn, double yposIn) {
    if (mouseGUIEnabled) {
        firstMouse = true;
        return;
    }

    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    // prevent initial jump in camera when mouse enters the window for the first time
    if (firstMouse){
        lastMousePosX = xpos;
        lastMousePosY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastMousePosX;
    float yoffset = -(ypos - lastMousePosY); // reversed since y coordinates go from bottom to top

    lastMousePosX = xpos;
    lastMousePosY = ypos;

    camera.processMouseMovement(xoffset, yoffset);
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    if (mouseGUIEnabled)
        return;

    camera.processMouseScroll(static_cast<float>(yoffset));
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_F && action == GLFW_PRESS) {
        enableFlashLight = !enableFlashLight;
    }

    if (key == GLFW_KEY_LEFT_ALT && action == GLFW_PRESS) {
        mouseGUIEnabled = !mouseGUIEnabled;
        glfwSetInputMode(window, GLFW_CURSOR, mouseGUIEnabled ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
    }

}


