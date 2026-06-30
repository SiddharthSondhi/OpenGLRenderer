#include "Shader.h"
#include "Camera.h"
#include "Mesh.h"
#include "Utils.h"
#include "VertexData.h"
#include "Model.h"
#include "SceneObject.h"
#include "Colors.h"
#include "GUI.h"
#include "Skybox.h"
#include "InstancedSceneObject.h"

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

Camera camera{ glm::vec3{30.0f, 5.0f, 15.0f } };

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

    //Shaders
    Shader simpleShader("./shaders/simpleVS.glsl", "./shaders/simpleFS.glsl");
    Shader lightShader("./shaders/lightVS.glsl", "./shaders/lightFS.glsl");
    Shader objectShader("./shaders/objectVS.glsl", "./shaders/objectFS.glsl");
    Shader depthShader("./shaders/depthVS.glsl", "./shaders/depthFS.glsl");
    Shader frameBufferShader("./shaders/frameBufferVS.glsl", "./shaders/frameBufferFS.glsl" );
    Shader skyboxShader("./shaders/skyboxVS.glsl", "./shaders/skyboxFS.glsl");
    Shader reflectiveShader("./shaders/reflectiveVS.glsl", "./shaders/reflectiveFS.glsl");
    Shader refractiveShader("./shaders/refractiveVS.glsl", "./shaders/refractiveFS.glsl");
    Shader explodeNormalsShader("./shaders/explodeNormalsVS.glsl", "./shaders/explodeNormalsFS.glsl", "./shaders/explodeNormalsGS.glsl");
    Shader normalVisShader("./shaders/normalsVS.glsl", "./shaders/normalsFS.glsl", "./shaders/normalsGS.glsl");
    Shader instanceObjectShader("./shaders/instanceObjectVS.glsl", "./shaders/objectFS.glsl");

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

    // textures
    unsigned int container2Diff{ Utils::loadTextureFromFile("./resources/textures/container2.png") };
    unsigned int container2Spec{ Utils::loadTextureFromFile("./resources/textures/container2_specular.png") };
    unsigned int boxMarbleTex{ Utils::loadTextureFromFile("./resources/textures/marble.jpg") };
    unsigned int planeMetalTex{ Utils::loadTextureFromFile("./resources/textures/metal.png") };
    unsigned int grassTex{ Utils::loadTextureFromFile("./resources/textures/grass.png") };
    unsigned int windowTex{ Utils::loadTextureFromFile("./resources/textures/blending_transparent_window.png")};

    std::array<unsigned int, 10> skyboxes  {
        0,
        Utils::loadCubemap("./resources/textures/cubemaps/SkyHighFluffyCloud"),
        Utils::loadCubemap("./resources/textures/cubemaps/PlanetaryEarth"),
        Utils::loadCubemap("./resources/textures/cubemaps/MegaSun"),
        Utils::loadCubemap("./resources/textures/cubemaps/highFantasy"),
        Utils::loadCubemap("./resources/textures/cubemaps/underTheSea"),
        Utils::loadCubemap("./resources/textures/cubemaps/CasualDay"),
        Utils::loadCubemap("./resources/textures/cubemaps/DayInTheClouds"),
        Utils::loadCubemap("./resources/textures/cubemaps/DarkStorm"),
        Utils::loadCubemap("./resources/textures/cubemaps/CoriolisNight")
    };

    // meshes and models
    Mesh containerMesh{ VertexData::cubeNormalsTexture, {3, 3, 2}, {{container2Diff, Texture::diffuse}, {container2Spec, Texture::specular}} };
    Mesh lightMesh{ VertexData::cubeTex, {3, 2} };
    Mesh marbleCubeMesh{ VertexData::cubeTex, {3, 2}, {{boxMarbleTex, Texture::diffuse}} };
    Mesh planeMesh{ VertexData::planeTex, {3, 2}, {{planeMetalTex, Texture::diffuse}} };
    Mesh grassMesh{ VertexData::transparent, {3, 2} , {{grassTex, Texture::diffuse}} };
    Mesh windowMesh{ VertexData::transparent, {3, 2}, {{windowTex, Texture::diffuse}} };
    Mesh screenQuadMesh{ VertexData::screenQuad, {2, 2}, {{textureColorbuffer, Texture::diffuse}} };

    Model backpackModel{ "./resources/models/backpack/backpack.obj", false };
    Model countrySceneModel{ "./resources/models/countryside-scene-free/source/untitled.glb", true };
    Model citySceneModel{ "./resources/models/city-scene/source/Untitled.glb", false };
    Model planetModel{ "./resources/models/planet/planet.obj", false };
    Model rockModel{ "./resources/models/rock/rock.obj", true };

    Skybox skybox{ VertexData::skyboxVertices, 0};

    // scene objects
    SceneObject backpack{ &backpackModel, backpackPos };
    backpack.scale = glm::vec3{ .8f };

    glm::vec3 lightPos{ 1.2f, 1.0f, 2.0f };
    glm::vec3 lightPos2{ -1.2f, 1.0f, 2.0f };
    SceneObject light1{ &lightMesh, lightPos , glm::vec3{0.2f} };
    SceneObject light2{ &lightMesh, lightPos2, glm::vec3{0.2f} };
    SceneObject light3{ &lightMesh, lightPos2, glm::vec3{0.2f} };

    SceneObject marbleCube1{ &marbleCubeMesh, {-1.0f, 0.0f, -31.0f } };
    SceneObject marbleCube2{ &marbleCubeMesh, { 2.0f, 0.0f, -30.0f } };

    SceneObject planeMetal{ &planeMesh , {0.0f, 0.0f, -30.0f} };
    planeMetal.scale = glm::vec3{ 1.5f, 1.0f, 1.5f };

    SceneObject countryScene{ &countrySceneModel, {50.0f, 0.0f, -30.0f} };
    countryScene.rotation = { 0.0f, 90.0f, 0.0f };

    SceneObject cityScene{ &citySceneModel};
    cityScene.rotation = { -90.0f, 0.0f, 0.0f };

    SceneObject planet{ &planetModel , {0.0f, -3.0f, 0.0f}, glm::vec3{4.0f} };

    std::vector<glm::vec3> vegetationPos{
        glm::vec3(-2.5f, 0.0f, -28.48f),
        glm::vec3(1.5f, 0.0f, -27.51f),
        glm::vec3(0.0f, 0.0f, -29.03f),
        glm::vec3(-0.3f, 0.0f, -32.3f),
        glm::vec3(0.5f, 0.0f, -30.6f)
    };

    std::vector<SceneObject> vegetation{};
    for (auto pos : vegetationPos) {
        vegetation.push_back(SceneObject{ &grassMesh, pos });
    }

    std::vector <glm::vec3> windowsPos{
        glm::vec3(-2.5f, 0.0f, -29.48f),
        glm::vec3(-.5f, 0.0f, -28.51f),
        glm::vec3(1.6f, 0.0f, -25.51f),
    };

    std::vector<SceneObject> windows{};
    for (auto pos : windowsPos) {
        windows.push_back(SceneObject{ &windowMesh, pos });
    }

    int amount{ 100000 };
    float radius{ 150.0f };
    float offset = { 25.0f };
    
    std::vector<glm::mat4> modelMats;
    for (int i{ 0 }; i < amount; i++) {
        float angle{ glm::radians(static_cast<float>(i) / amount * 360.0f )};
        float displacement{ (rand() % static_cast<int>(2 * offset * 100)) / 100.0f - offset };
        float x{ sin(angle) * radius + displacement };
        displacement = (rand() % static_cast<int>(2 * offset * 100)) / 100.0f - offset;
        float y{ displacement * 0.4f };
        displacement = (rand() % static_cast<int>(2 * offset * 100)) / 100.0f - offset;
        float z{ cos(angle) * radius + displacement };
        float scale = (rand() % 20) / 100.0f + 0.05;
        float rotAngle = (rand() % 360);
        
        glm::mat4 mat{ 1.0f };
        mat = glm::translate(mat, { x, y, z });
        mat = glm::rotate(mat, rotAngle, glm::vec3{ 0.4f, 0.6f, 0.8f });
        mat = glm::scale(mat, glm::vec3{ scale });

        modelMats.push_back(mat);
    }

    InstancedSceneObject asteroids{ &rockModel, modelMats };


    // ----------------------------------------Uniforms----------------------------------------------
    objectShader.use();
    
    objectShader.setFloat("material.shininess", 256.0f);

    // directional light
    objectShader.setVec3("dirLight.ambient", { 0.2f, 0.2f, 0.2f });
    objectShader.setVec3("dirLight.diffuse", { 0.7f, 0.7f, 0.7f });
    objectShader.setVec3("dirLight.specular", { 1.0f, 1.0f, 1.0f });

    // point lights
    objectShader.setFloat("pointLights[0].constant", 1.0f);
    objectShader.setFloat("pointLights[0].linear", 0.09f);
    objectShader.setFloat("pointLights[0].quadratic", 0.032f);
    objectShader.setVec3("pointLights[0].diffuse", Colors::blue);
    objectShader.setVec3("pointLights[0].specular", Colors::blue);

    objectShader.setFloat("pointLights[1].constant", 1.0f);
    objectShader.setFloat("pointLights[1].linear", 0.09f);
    objectShader.setFloat("pointLights[1].quadratic", 0.032f);
    objectShader.setVec3("pointLights[1].diffuse", Colors::red);
    objectShader.setVec3("pointLights[1].specular", Colors::red);

    objectShader.setFloat("pointLights[2].constant", 1.0f);
    objectShader.setFloat("pointLights[2].linear", 0.09f);
    objectShader.setFloat("pointLights[2].quadratic", 0.032f);
    objectShader.setVec3("pointLights[2].diffuse", Colors::green);
    objectShader.setVec3("pointLights[2].specular", Colors::green);

    // spot light
    objectShader.setFloat("spotLight.constant", 1.0f);
    objectShader.setFloat("spotLight.linear", 0.022f);
    objectShader.setFloat("spotLight.quadratic", 0.0019f);
    objectShader.setVec3("spotLight.ambient",  { 0.2f, 0.2f, 0.2f });
    objectShader.setVec3("spotLight.diffuse",  { 0.5f, 0.5f, 0.5f });
    objectShader.setVec3("spotLight.specular", { 1.0f, 1.0f, 1.0f });
    objectShader.setFloat("spotLight.innerCutOff", glm::cos(glm::radians(12.5f)));
    objectShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(17.5f)));


    instanceObjectShader.use();
    
    instanceObjectShader.setFloat("material.shininess", 256.0f);

    // directional light
    instanceObjectShader.setVec3("dirLight.ambient", { 0.2f, 0.2f, 0.2f });
    instanceObjectShader.setVec3("dirLight.diffuse", { 0.7f, 0.7f, 0.7f });
    instanceObjectShader.setVec3("dirLight.specular", { 1.0f, 1.0f, 1.0f });

    // point lights
    instanceObjectShader.setFloat("pointLights[0].constant", 1.0f);
    instanceObjectShader.setFloat("pointLights[0].linear", 0.09f);
    instanceObjectShader.setFloat("pointLights[0].quadratic", 0.032f);
    instanceObjectShader.setVec3("pointLights[0].diffuse", Colors::blue);
    instanceObjectShader.setVec3("pointLights[0].specular", Colors::blue);

    instanceObjectShader.setFloat("pointLights[1].constant", 1.0f);
    instanceObjectShader.setFloat("pointLights[1].linear", 0.09f);
    instanceObjectShader.setFloat("pointLights[1].quadratic", 0.032f);
    instanceObjectShader.setVec3("pointLights[1].diffuse", Colors::red);
    instanceObjectShader.setVec3("pointLights[1].specular", Colors::red);

    instanceObjectShader.setFloat("pointLights[2].constant", 1.0f);
    instanceObjectShader.setFloat("pointLights[2].linear", 0.09f);
    instanceObjectShader.setFloat("pointLights[2].quadratic", 0.032f);
    instanceObjectShader.setVec3("pointLights[2].diffuse", Colors::green);
    instanceObjectShader.setVec3("pointLights[2].specular", Colors::green);

    // spot light
    instanceObjectShader.setFloat("spotLight.constant", 1.0f);
    instanceObjectShader.setFloat("spotLight.linear", 0.022f);
    instanceObjectShader.setFloat("spotLight.quadratic", 0.0019f);
    instanceObjectShader.setVec3("spotLight.ambient", { 0.2f, 0.2f, 0.2f });
    instanceObjectShader.setVec3("spotLight.diffuse", { 0.5f, 0.5f, 0.5f });
    instanceObjectShader.setVec3("spotLight.specular", { 1.0f, 1.0f, 1.0f });
    instanceObjectShader.setFloat("spotLight.innerCutOff", glm::cos(glm::radians(12.5f)));
    instanceObjectShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(17.5f)));

    
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

        //update postions
        orbitLights(light1, light2, light3);


        // calculate view and projection matrix
        glm::mat4 view{ camera.getViewMatrix() };
        glm::mat4 projection{ glm::perspective(glm::radians(camera.zoom), static_cast<float>(windowWidth) / windowHeight, 0.1f, 500.0f) };

        glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(view));
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projection));


        // post processing 
        frameBufferShader.use();
        frameBufferShader.setFloat("offset", 1.0f / gui.convMatrixOffset);
        frameBufferShader.setInt("postProcessingMode", gui.postProcessingMode);


        //lights
        lightShader.use();

        lightShader.setVec3("color", Colors::blue);
        //light1.draw(lightShader);

        lightShader.setVec3("color", Colors::red);
        //light2.draw(lightShader);

        lightShader.setVec3("color", Colors::green);
        //light3.draw(lightShader);

        // object shader
        objectShader.use();

        objectShader.setVec3("dirLight.direction", glm::vec3{ view * glm::vec4{ -0.3f, -1.0f, -0.2f , 0.0f} });
        objectShader.setVec3("pointLights[0].position", glm::vec3{ view * glm::vec4{light1.position, 1.0f} });
        objectShader.setVec3("pointLights[1].position", glm::vec3{ view * glm::vec4{light2.position, 1.0f} });
        objectShader.setVec3("pointLights[2].position", glm::vec3{ view * glm::vec4{light3.position, 1.0f} });
        objectShader.setVec3("spotLight.position", glm::vec3{ 0.0f });
        objectShader.setVec3("spotLight.direction", glm::vec3{ view * glm::vec4{camera.front, 0.0f } });
        objectShader.setBool("enableFlashLight", enableFlashLight);

        instanceObjectShader.use();

        instanceObjectShader.setVec3("dirLight.direction", glm::vec3{ view * glm::vec4{ -0.3f, -1.0f, -0.2f , 0.0f} });
        instanceObjectShader.setVec3("pointLights[0].position", glm::vec3{ view * glm::vec4{light1.position, 1.0f} });
        instanceObjectShader.setVec3("pointLights[1].position", glm::vec3{ view * glm::vec4{light2.position, 1.0f} });
        instanceObjectShader.setVec3("pointLights[2].position", glm::vec3{ view * glm::vec4{light3.position, 1.0f} });
        instanceObjectShader.setVec3("spotLight.position", glm::vec3{ 0.0f });
        instanceObjectShader.setVec3("spotLight.direction", glm::vec3{ view * glm::vec4{camera.front, 0.0f } });
        instanceObjectShader.setBool("enableFlashLight", enableFlashLight);

        //explode normals
        explodeNormalsShader.use();
        explodeNormalsShader.setFloat("time", static_cast<float>(glfwGetTime()));


        //draw objects
        //backpack.draw(objectShader);
        //backpack.draw(normalVisShader);
        //cityScene.draw(objectShader);
        //countryScene.draw(objectShader);
        
        planet.draw(objectShader);
        asteroids.draw(instanceObjectShader);
        
        // other objects
        simpleShader.use();

        //.draw(depthShader);
        //marbleCube2.draw(simpleShader);
        //planeMetal.draw(simpleShader);

        //transparent
        for (auto& v : vegetation) {
            //v.draw(simpleShader);
        }
        
        // semi transparent
        std::sort(windows.begin(), windows.end(),
            [](const auto& a, const auto& b) {
                return glm::length2(camera.position - a.position) > glm::length2(camera.position - b.position);
            }
        );

        for (auto& w : windows) {
            //w.draw(simpleShader);
        }

        //skybox
        skybox.texture = skyboxes[gui.skyboxIndex];
        skybox.draw(skyboxShader);

        // now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
        
        // clear all relevant buffers
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // draw screen quad (postprocessing)
        screenQuadMesh.draw(frameBufferShader);

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

void orbitLights(SceneObject& light1, SceneObject& light2, SceneObject& light3) {
    float radius = 2.5f;
    float t = static_cast<float>(glfwGetTime()) * 2.0f;

    // Base orbit positions (120 degrees apart)
    glm::vec3 p1{
        sin(t) * radius,
        0.0f,
        cos(t) * radius
    };

    glm::vec3 p2{
        sin(t + 2.0943951f) * radius,
        0.0f,
        cos(t + 2.0943951f) * radius
    };

    glm::vec3 p3{
        sin(t + 4.1887902f) * radius,
        0.0f,
        cos(t + 4.1887902f) * radius
    };

    // Different orbital planes
    glm::mat4 tilt1 = glm::rotate(
        glm::mat4(1.0f),
        glm::radians(30.0f),
        glm::vec3(0.0f, 0.0f, 1.0f)
    );

    glm::mat4 tilt2 = glm::rotate(
        glm::mat4(1.0f),
        glm::radians(30.0f),
        glm::vec3(0.0f, 0.0f, -1.0f)
    );

    glm::mat4 tilt3 = glm::rotate(
        glm::mat4(1.0f),
        glm::radians(30.0f),
        glm::vec3(1.0f, 0.0f, 0.0f)
    );

    glm::vec3 center{ backpackPos + glm::vec3{0.0f, 1.0f, 0.0f} };

    light1.position = glm::vec3(tilt1 * glm::vec4(p1, 1.0f)) + center;
    light2.position = glm::vec3(tilt2 * glm::vec4(p2, 1.0f)) + center;
    light3.position = glm::vec3(tilt3 * glm::vec4(p3, 1.0f)) + center;
}

