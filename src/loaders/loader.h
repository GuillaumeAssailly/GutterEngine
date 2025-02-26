#pragma once
#include "../config.h"
#include "../controllers/app.h"

class Loader {
    protected :
        virtual void loadPins(App* app, CameraComponent* camera) const = 0;
        virtual void loadBall(App* app, CameraComponent* camera) const = 0;
        virtual void loadLights(App *app, CameraComponent* camera) const = 0;
        virtual void loadCamera(App* app, CameraComponent* camera) const = 0;
        virtual void loadLane(App* app, CameraComponent* camera) const = 0;
		//virtual void loadTransform(App* app, CameraComponent* camera) const = 0;
    public :
        virtual void loadEntities(App *app) const;
        virtual ~Loader() {}
};