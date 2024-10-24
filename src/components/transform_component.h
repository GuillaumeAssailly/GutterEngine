#pragma once
#include "../config.h"

struct TransformComponent {
    glm::vec3 position;
    glm::quat eulers;
    glm::vec3 size;
};