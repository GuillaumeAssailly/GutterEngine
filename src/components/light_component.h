#pragma once
#include "../config.h"
#include "../loaders/json.hpp"
#include "../loaders/json_glm_utils.hpp"

enum LightType {
	POINT,
	DIRECTIONAL,
	SPOT
};

inline std::string LightTypeToString(LightType type) {
    switch (type) {
    case POINT: return "POINT";
    case DIRECTIONAL: return "DIRECTIONAL";
    case SPOT: return "SPOT";
    default: return "UNKNOWN";
    }
}

inline LightType StringToLightType(const std::string& str) {
    static const std::unordered_map<std::string, LightType> mapping = {
        {"POINT", POINT},
        {"DIRECTIONAL", DIRECTIONAL},
        {"SPOT", SPOT}
    };

    auto it = mapping.find(str);
    return (it != mapping.end()) ? it->second : POINT; // Par défaut `POINT`
}

inline void to_json(json& j, const LightType& type) {
    j = LightTypeToString(type);
}


inline void from_json(const json& j, LightType& type) {
    type = StringToLightType(j.get<std::string>());
}

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

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(LightComponent, color, intensity, direction, type, lightSpaceMatrix, cutoff, outerCutoff, shadowMapLayer)