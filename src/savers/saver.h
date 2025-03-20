#pragma once
#include "../controllers/app.h"
class Saver
{
protected:
    virtual void saveTransforms() = 0;
    virtual void saveRenders() = 0;
	virtual void saveCameras() = 0;
	virtual void saveLights() = 0;
	virtual void saveEntitiesNames() = 0;
	virtual void savePhysics() = 0;

public:
    virtual void saveEntities() = 0;
    virtual ~Saver() {}

};
