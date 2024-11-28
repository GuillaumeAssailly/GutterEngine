#pragma once
#include "../config.h"

struct CameraComponent {
    float fov;
    float aspectRatio;
    float nearPlane;
    float farPlane;
    float sensitivity;
    glm::vec4 initialForward;
};