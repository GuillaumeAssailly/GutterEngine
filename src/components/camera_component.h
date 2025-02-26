#pragma once
#include "../config.h"
#include "../loaders/json.hpp"
#include "../loaders/json_glm_utils.hpp"

using json = nlohmann::json;



struct CameraComponent {
    float fov;
    float aspectRatio;
    float nearPlane;
    float farPlane;
    float sensitivity;
    glm::vec4 initialForward;

};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(CameraComponent, fov, aspectRatio, nearPlane, farPlane, sensitivity, initialForward)
