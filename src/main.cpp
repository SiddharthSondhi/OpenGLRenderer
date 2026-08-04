#include "Shader.h"
#include "Camera.h"
#include "GUI.h"
#include "Scene.h"
#include "Resources.h"
#include "SceneBuilder.h"
#include "Renderer.h"

#include <glad/glad.h> 
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <iostream>
#include <array>

//prototypes
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

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
GUI::Settings gui;

Camera camera{ glm::vec3{0.0f, 0.0f, 15.0f } };

bool enableFlashLight{ false };


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
    
    //Renderer
    Renderer renderer;
    renderer.init(windowWidth, windowHeight);

    // scene objects
    Resources resources;

    Scene mainScene{ buildMainScene(resources) };
    Scene planetScene{ buildPlanetScene(resources) };
    Scene cityScene{ buildCityScene(resources) };
    Scene countryScene{ buildCountryScene(resources) };
    Scene refScene{ buildRefScene(resources) };
    Scene lightScene{ buildLightScene(resources) };
    Scene shadowScene{ buildShadowScene(resources) };
    Scene normalMapScene{ buildNormalMapScene(resources) };

    std::array<Scene*,  8> scenes{ &mainScene, &planetScene, &cityScene, &countryScene, &refScene, &lightScene, &shadowScene, &normalMapScene };

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

        //update scene
        Scene* currentScene{ scenes[gui.currentSceneIndex] };
        gui.dirLightDirection = &currentScene->dirLight.direction;
        renderer.updateMatrices(windowWidth, windowHeight);
        currentScene->update(deltaTime);
        
        //update light data
        renderer.updateLightData(*currentScene, enableFlashLight);

        //render scene
        renderer.render(windowWidth, windowHeight, *currentScene, resources);

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

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods){
    if (key == GLFW_KEY_F && action == GLFW_PRESS) {
        enableFlashLight = !enableFlashLight;
    }

    if (key == GLFW_KEY_LEFT_ALT && action == GLFW_PRESS) {
        mouseGUIEnabled = !mouseGUIEnabled;
        glfwSetInputMode(window, GLFW_CURSOR, mouseGUIEnabled ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
    }
}


