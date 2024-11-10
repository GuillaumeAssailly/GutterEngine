#pragma once
#include "../config.h"

struct RenderComponent {
    unsigned int material;
    unsigned int mesh;
    unsigned int indexCount;
    unsigned int normalMap = 0;
};