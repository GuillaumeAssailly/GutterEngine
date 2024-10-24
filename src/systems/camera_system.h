#pragma once
#include "../config.h"
#include "../components/camera_component.h"
#include "../components/transform_component.h"

class CameraSystem {
public:

    CameraSystem(unsigned int shader, GLFWwindow* window);

    bool update(
        std::unordered_map<unsigned int, TransformComponent>& transformComponents,
        unsigned int cameraID, CameraComponent& cameraComponent, float dt);

private:
    unsigned int viewLocation;

    bool moving = false;
    double mouse_x_ref = .0f;
    double mouse_y_ref = .0f;

    glm::vec3 global_up = { 0.0f, 1.0f, 0.0f };
    GLFWwindow* window;
};