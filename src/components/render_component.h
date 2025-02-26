#pragma once
#include "../config.h"
#include "../loaders/json.hpp"

struct RenderComponent {
    unsigned int material;
    unsigned int mesh;
    unsigned int indexCount;
    unsigned int normalMap = 0;
	unsigned int emissiveMap = 0;
    unsigned int aoMap = 0;
	unsigned int metalnessRoughnessMap = 0;
    float emissiveForce = 8.0f;
    bool isPlanarReflectable = false;
    unsigned int reflectionFBO;
    unsigned int reflectionTexture;
	unsigned int reflectionDepthBuffer;


    NLOHMANN_DEFINE_TYPE_INTRUSIVE(RenderComponent, material, mesh, indexCount, normalMap, emissiveMap, aoMap, 
        metalnessRoughnessMap, emissiveForce, isPlanarReflectable, reflectionFBO, reflectionTexture, reflectionDepthBuffer)
};

