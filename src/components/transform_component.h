#pragma once
#include "../config.h"
#include "../loaders/json.hpp"
#include "../loaders/json_glm_utils.hpp"

struct TransformComponent {
    glm::vec3 position;
    glm::quat eulers;
    glm::vec3 size;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TransformComponent, position, eulers, size)