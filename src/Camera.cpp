#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>


Camera::Camera(glm::vec3 position, glm::vec3 worldUp, float yaw, float pitch) :
    position{ position },
    worldUp{ worldUp },
    yaw{ yaw },
    pitch{ pitch } 
{
    updateCameraVectors();
}

void Camera::updateCameraVectors() {

    // calculate the new Front vector
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    this->front = glm::normalize(front);

    // also re-calculate the Right vector
    // normalize the vector, because its length gets closer to 0 the more you look up or down which results in slower movement.
    right = glm::normalize(glm::cross(front, worldUp));  
}

glm::mat4 Camera::getViewMatrix() {
    return glm::lookAt(position, position + front, worldUp);
}

void Camera::processKeyboard(Camera_Movement direction, float deltaTime){
    float velocity = movementSpeed * deltaTime;
    if (direction == FORWARD)
        position += front * velocity;
    if (direction == BACKWARD)
        position -= front * velocity;
    if (direction == RIGHT)
        position += right * velocity;
    if (direction == LEFT)
        position -= right * velocity;
    if (direction == UP)
        position += worldUp * velocity;
    if (direction == DOWN)
        position -= worldUp * velocity;
}

void Camera::processMouseMovement(float xoffset, float yoffset, bool constrainPitch) {
    yaw += xoffset * mouseSensitivity;
    pitch += yoffset * mouseSensitivity;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch){
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;
    }

    // update Front and Right Vectors using the updated Euler angles
    updateCameraVectors();
}

void Camera::processMouseScroll(float yoffset){
    constexpr float MAX_FOV = 160;

    zoom -= yoffset * 2.0f;
    if (zoom < 1.0f)
        zoom = 1.0f;
    if (zoom > MAX_FOV)
        zoom = MAX_FOV;
}