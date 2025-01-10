#pragma once
#include "../controllers/app.h"
class Saver
{
protected:
    virtual void saveQuilles(App* app, CameraComponent* camera) = 0;
    virtual void saveBall(App* app, CameraComponent* camera) = 0;
    virtual void saveLight(App* app, CameraComponent* camera) = 0;
    virtual void saveCamera(App* app, CameraComponent* camera) = 0;
    virtual void saveLane(App* app, CameraComponent* camera) = 0;
public:
    virtual void saveEntities(App* app);
    virtual ~Saver() {}

};
