#pragma once
#include "../config.h"

struct CameraComponent {
    glm::vec3 right;
    glm::vec3 up;
    glm::vec3 forwards;
	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;
};