#pragma once
#include "../config.h"
#include "../components/camera_component.h"
#include "../components/transform_component.h"

class CameraSystem {
public:

    CameraSystem(unsigned int shader, GLFWwindow* window);

    bool update(
        std::unordered_map<unsigned int, TransformComponent>& transformComponents,
        std::unordered_map<unsigned int, CameraComponent>& cameraComponents,
        unsigned int cameraID, float dt);

    glm::mat4 GetViewMatrix();
    glm::mat4 GetProjectionMatrix();

private:
    unsigned int viewLocation;
    GLint projectionLocation;
    float baseSpeed;
    float speedMultiplier;
    bool moving = false;

    bool firstMouse = true;
    double mouse_x_ref = .0f;
    double mouse_y_ref = .0f;

    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;

    glm::vec3 global_up = { 0.f, 1.f, 0.f };
    glm::vec3 dPos = { 0.f, 0.f, 0.f };
    GLFWwindow* window;
};