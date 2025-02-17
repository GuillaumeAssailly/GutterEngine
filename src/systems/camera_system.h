#pragma once
#include "../config.h"
#include "../components/camera_component.h"
#include "../components/transform_component.h"

class CameraSystem {
public:

    CameraSystem(unsigned int shader, GLFWwindow* window);

    void update(
        std::unordered_map<unsigned int, TransformComponent>& transformComponents,
        std::unordered_map<unsigned int, CameraComponent>& cameraComponents,
        unsigned int cameraID, float dt);

    glm::vec3 getForward();
    glm::mat4 GetViewMatrix();
    glm::mat4 GetProjectionMatrix();

private:
    unsigned int viewLocation;
    GLint projectionLocation;

    bool firstMouse = true;

    bool moving = false;
    double mouse_x_ref = .0f;
    double mouse_y_ref = .0f;

    glm::vec3 forward;
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;

    glm::vec3 global_up = { 0.0f, 1.0f, 0.0f };
    GLFWwindow* window;

friend class InputManager;
};