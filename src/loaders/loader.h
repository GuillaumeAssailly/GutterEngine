#pragma once
#include "../config.h"

class Loader {
public:
    virtual void load(glm::vec3* vectors) const = 0; // Méthode virtuelle pure
    virtual ~Loader() {}
};