#pragma once
#include "../config.h"

struct RenderComponent {
    std::vector<unsigned int> materials;
    std::vector<unsigned int> meshes;
    std::vector<unsigned int> indexCount;
    std::vector<unsigned int> normalMap;
};