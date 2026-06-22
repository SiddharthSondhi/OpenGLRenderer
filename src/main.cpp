#include "Shader.h";
#include "Camera.h"
#include "Mesh.h"
#include "Utils.h"
#include "VertexData.h"
#include "Model.h"
#include "SceneObject.h"
#include "Colors.h"

#include <glad/glad.h> 
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>


//prototypes
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

void orbitLights(SceneObject& light1, SceneObject& light2, SceneObject& light3);


//global variables
constexpr int WINDOW_WIDTH{ 1920 };
constexpr int WINDOW_HEIGHT{ 1080 };

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

// position of mouse last frame, used to calculate yaw and pitch for camera when moving mouse
float lastMousePosX{ WINDOW_WIDTH / 2 };
float lastMousePosY{ WINDOW_HEIGHT / 2 };  
bool firstMouse{ true };

Camera camera{ glm::vec3{0.0f, 0.0f, 5.0f } };

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

    GLFWwindow* window{ glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "OpenGL Window", NULL, NULL) };
    if (window == NULL) {
        std::cout << "Failed to create GLFW window.\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


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

    //Shaders
    Shader basicShader("./shaders/simpleVS.glsl", "./shaders/simpleFS.glsl");
    Shader lightShader("./shaders/lightVS.glsl", "./shaders/lightFS.glsl");
    Shader objectShader("./shaders/objectVS.glsl", "./shaders/objectFS.glsl");

    // textures
    unsigned int boxDiffTex{ Utils::loadTexture("./resources/textures/container2.png") };
    unsigned int boxSpecTex{ Utils::loadTexture("./resources/textures/container2_specular.png") };

    // meshes and models
    Mesh containerMesh{ VertexData::cubeNormalsTexture, {3, 3, 2}, {{boxDiffTex, Texture::diffuse}, {boxSpecTex, Texture::specular}} };
    Mesh lightMesh{ VertexData::cube, {3} };
    Model backpackModel{ "./resources/models/backpack/backpack.obj", false };

    // scene objects
    SceneObject backpack{ &backpackModel };
    backpack.scale = glm::vec3{ .8f };

    glm::vec3 lightPos{ 1.2f, 1.0f, 2.0f };
    glm::vec3 lightPos2{ -1.2f, 1.0f, 2.0f };
    SceneObject light1{ &lightMesh, lightPos , glm::vec3{0.2f} };
    SceneObject light2{ &lightMesh, lightPos2, glm::vec3{0.2f} };
    SceneObject light3{ &lightMesh, lightPos2, glm::vec3{0.2f} };

    // ----------------------------------------Uniforms----------------------------------------------
    objectShader.use();
    
    // material
    objectShader.setVec3("material.ambient", { 1.0f, 0.5f, 0.31f });
    objectShader.setVec3("material.diffuse", { 1.0f, 0.5f, 0.31f });
    objectShader.setVec3("material.specular", { 0.5f, 0.5f, 0.5f });
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

    


    // settings
    glEnable(GL_DEPTH_TEST);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //wireframe mode

    // ---------------------------------------Rendering Loop-------------------------------------------
    while (!glfwWindowShouldClose(window)) {
        // time logic
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input processing
        glfwPollEvents();
        processInput(window);


        //rendering
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //update postions
        orbitLights(light1, light2, light3);


        // calculate view and projection matrix
        glm::mat4 view{ camera.getViewMatrix() };
        glm::mat4 projection{ glm::perspective(glm::radians(camera.zoom), static_cast<float>(WINDOW_WIDTH) / WINDOW_HEIGHT, 0.1f, 100.0f) };


        // object
        objectShader.use();

        objectShader.setMat4("view", view);
        objectShader.setMat4("projection", projection);

        objectShader.setVec3("dirLight.direction", glm::vec3{ view * glm::vec4{ -0.3f, -1.0f, -0.2f , 0.0f} });
        objectShader.setVec3("pointLights[0].position", glm::vec3{ view * glm::vec4{light1.position, 1.0f} });
        objectShader.setVec3("pointLights[1].position", glm::vec3{ view * glm::vec4{light2.position, 1.0f} });
        objectShader.setVec3("pointLights[1].position", glm::vec3{ view * glm::vec4{light3.position, 1.0f} });
        objectShader.setVec3("spotLight.position", glm::vec3{ view * glm::vec4{camera.position, 1.0f } });
        objectShader.setVec3("spotLight.direction", glm::vec3{ view * glm::vec4{camera.front, 0.0f } });
        objectShader.setBool("enableFlashLight", enableFlashLight);

        backpack.draw(objectShader);

        //lights
        lightShader.use();
        lightShader.setMat4("view", view);
        lightShader.setMat4("projection", projection);

        lightShader.setVec3("color", Colors::blue);
        light1.draw(lightShader);

        lightShader.setVec3("color", Colors::red);
        light2.draw(lightShader);

        lightShader.setVec3("color", Colors::green);
        light3.draw(lightShader);


        glfwSwapBuffers(window);
    }

    // cleanup
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
}

void mouseCallback(GLFWwindow* window, double xposIn, double yposIn) {
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
    camera.processMouseScroll(yoffset);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_F && action == GLFW_PRESS) {
        enableFlashLight = !enableFlashLight;
    }

}

void orbitLights(SceneObject& light1, SceneObject& light2, SceneObject& light3) {
    //float lightOrbitRadius = 3.5f;

    //glm::vec3 basePos;
    //basePos.x = sin(glfwGetTime() * 2) * lightOrbitRadius;
    //basePos.y = 0.0f;
    //basePos.z = cos(glfwGetTime() * 2) * lightOrbitRadius;
    //glm::mat4 tilt = glm::rotate(glm::mat4(1.0f), glm::radians(30.0f), glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f)));
    //light1.position = glm::vec3(tilt * glm::vec4(basePos, 1.0f)) + glm::vec3(0.0f);

    //basePos.x = sin((glfwGetTime() + 2.14) * 2) * lightOrbitRadius;
    //basePos.y = 0.0f;
    //basePos.z = cos((glfwGetTime() + 2.14) * 2) * lightOrbitRadius;
    //tilt = glm::rotate(glm::mat4(1.0f), glm::radians(30.0f), glm::normalize(glm::vec3(0.0f, 0.0f, -1.0f)));
    //light2.position = glm::vec3(tilt * glm::vec4(basePos, 1.0f)) + glm::vec3(0.0f);

    float radius = 3.5f;
    float t = glfwGetTime() * 2.0f;

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

    glm::vec3 center{ 0.0f, 1.0f, 0.0f };

    light1.position = glm::vec3(tilt1 * glm::vec4(p1, 1.0f)) + center;
    light2.position = glm::vec3(tilt2 * glm::vec4(p2, 1.0f)) + center;
    light3.position = glm::vec3(tilt3 * glm::vec4(p3, 1.0f)) + center;
}