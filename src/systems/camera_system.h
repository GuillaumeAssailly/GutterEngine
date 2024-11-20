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

    double lastMouseX = 0.0, lastMouseY = 0.0; // Dernières positions de la souris
    bool firstMouseMovement = true;

    float pitchTotal = 0;
    bool firstMouse = true;
    float deltaX = 0, deltaY = 0;
    float lastX = 0.0f, lastY = 0.0f;

    bool moving = false;
    double mouse_x_ref = .0f;
    double mouse_y_ref = .0f;

    glm::vec3 global_up = { 0.0f, 1.0f, 0.0f };
    GLFWwindow* window;
};