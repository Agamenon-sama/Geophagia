#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Necrosis {

enum class CameraMovement {
    Forward,
    Back,
    Up,
    Down,
    Left,
    Right
};

class Camera {
public:
    // Camera() = default;
    Camera(const glm::vec3 &pos = glm::vec3(0.f, 0.f, -1.f),
           float aspectRatio = 1.77, float fov = 45.f,
           float near = 0.1f, float far = 100.f);

    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjMatrix() const;
    void processKeyboard(CameraMovement mov, float deltaTime);
    void processMouse(float xoffset, float yoffset);

protected:
    void _updateCameraVectors();

public:
    // camera
    glm::vec3 position;
    glm::vec3 direction;

    glm::vec3 worldUp;

    // camera movement controls
    float sensitivity;
    float movementSpeed;

    // camera movement
    glm::vec3 front; // vector by which we move when we move forward
    glm::vec3 right; // vector by which we move when we move right
    glm::vec3 up; // vector by which we move when we move up

    // for projection
    float aspectRatio;
    float fov;
    float fovModifier;
    float near;
    float far;

protected:
    // camera rotation
    float _yaw;
    float _pitch;
};

}
