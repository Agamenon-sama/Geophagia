#include "Camera.h"

namespace Necrosis {

Camera::Camera(const glm::vec3 &pos, float aspectRatio, float fov, float near, float far)
    : position(pos), worldUp(glm::vec3(0.f, 1.f, 0.f)), front(glm::vec3(0.0f, 0.0f, -1.0f))
    , aspectRatio(aspectRatio), fov(fov), near(near), far(far) {
    
    movementSpeed = 2.f;
    sensitivity = 0.1f;
    fovModifier = 2.f;

    _yaw = -90.f;
    _pitch = 0.f;

    _updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(position, position + front, worldUp);
}

glm::mat4 Camera::getProjMatrix() const {
    return glm::perspective(glm::radians(fov), aspectRatio, near, far);
}

void Camera::processKeyboard(CameraMovement mov, float deltaTime) {
    float velocity = movementSpeed * deltaTime;
    switch (mov) {
    case CameraMovement::Forward:
        position += front * velocity;
        break;
    case CameraMovement::Back:
        position -= front * velocity;
        break;
    case CameraMovement::Right:
        position += right * velocity;
        break;
    case CameraMovement::Left:
        position -= right * velocity;
        break;
    case CameraMovement::Up:
        position += up * velocity;
        break;
    case CameraMovement::Down:
        position -= up * velocity;
        break;
    }
}

void Camera::processMouse(float xoffset, float yoffset) {
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    _yaw += xoffset;
    _pitch += yoffset;

    // clamp pitch so the camera doesn't flip
    if (_pitch > 89.f) _pitch = 89.f;
    if (_pitch < -89.f) _pitch = -89.f;

    _updateCameraVectors();
}


void Camera::_updateCameraVectors() {
    // get the front vector after changing the angles
    front.x = std::cos(glm::radians(_yaw)) * std::cos(glm::radians(_pitch));
    front.y = std::sin(glm::radians(_pitch));
    front.z = std::sin(glm::radians(_yaw)) * std::cos(glm::radians(_pitch));
    front = glm::normalize(front);

    right = glm::normalize(glm::cross(front, worldUp));
    up    = glm::normalize(glm::cross(right, front));
}

}
