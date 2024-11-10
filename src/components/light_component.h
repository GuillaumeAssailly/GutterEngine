#pragma once
#include "../config.h"

struct LightComponent {
	glm::vec3 color;
	float intensity;
	glm::vec3 direction;
	unsigned int shadowFBO;
	unsigned int depthMap;
	bool isDirectional = false;
	glm::mat4 lightSpaceMatrix;

};