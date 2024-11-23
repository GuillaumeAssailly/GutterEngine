#pragma once
#include "../config.h"
#include "../controllers/app.h"

class Loader {
public:
    virtual void loadQuilles(App *app, CameraComponent *camera) const = 0;
    virtual void loadEntities(App *app) const = 0;
    virtual ~Loader() {}
};