#pragma once
#include "../config.h"
#include "../controllers/app.h"

class Loader {
    protected :
        virtual void loadQuilles(App* app, CameraComponent* camera) const = 0;
        virtual void loadBall(App* app, CameraComponent* camera) const = 0;
        virtual void loadLight(App *app, CameraComponent* camera) const = 0;
        virtual void loadCamera(App* app, CameraComponent* camera) const = 0;
        virtual void loadLane(App* app, CameraComponent* camera) const = 0;
    public :
        virtual void loadEntities(App *app) const;
        virtual ~Loader() {}
};