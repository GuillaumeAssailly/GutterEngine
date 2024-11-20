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

private:
    unsigned int viewLocation;
    GLint projectionLocation;

    bool firstMouse = true;

    bool moving = false;
    double mouse_x_ref = .0f;
    double mouse_y_ref = .0f;

    GLFWwindow* window;
};