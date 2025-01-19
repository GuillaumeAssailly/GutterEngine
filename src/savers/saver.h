#pragma once
#include "../controllers/app.h"
class Saver
{
protected:
    virtual void savePins(App* app) = 0;
    virtual void saveBall(App* app) = 0;
    virtual void saveLights(App* app) = 0;
    virtual void saveCamera(App* app) = 0;
    virtual void saveLane(App* app) = 0;
public:
    virtual void saveEntities(App* app);
    virtual ~Saver() {}

};
