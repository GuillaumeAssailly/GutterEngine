#pragma once
#include "../config.h"

enum LightType {
	POINT,
	DIRECTIONAL,
	SPOT
};

struct LightComponent {
	glm::vec3 color;
	float intensity;
	glm::vec3 direction;
	//unsigned int shadowFBO;
	//unsigned int depthMap;
	LightType type = LightType::POINT;
	glm::mat4 lightSpaceMatrix;
	float cutoff = 0.0f;
	float outerCutoff = 0.0f;

	unsigned int shadowMapLayer;

};