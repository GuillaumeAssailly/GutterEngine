#pragma once
#include "../config.h"

struct RenderComponent {
    unsigned int material;
    unsigned int mesh;
    unsigned int indexCount;
    unsigned int normalMap = 0;
	unsigned int emissiveMap = 0;
    unsigned int aoMap = 0;
    float emissiveForce = 8.0f;
    bool isPlanarReflectable = false;
    unsigned int reflectionFBO;
    unsigned int reflectionTexture;
	unsigned int reflectionDepthBuffer;
};