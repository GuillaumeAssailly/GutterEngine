#pragma once
#include "../controllers/app.h"
class Saver
{
protected:
    /*virtual void savePins(App* app) = 0;
    virtual void saveBall(App* app) = 0;
    virtual void saveLights(App* app) = 0;
    virtual void saveCamera(App* app) = 0;
    virtual void saveLane(App* app) = 0;*/
    virtual void saveTransforms(EntityManager entityManager) = 0;
    virtual void saveRenders(EntityManager entityManager) = 0;
	virtual void saveCameras(EntityManager entityManager) = 0;
	virtual void saveLights(EntityManager entityManager) = 0;
	virtual void saveEntitiesNames(EntityManager entityManager) = 0;
	virtual void savePhysics(EntityManager entityManager) = 0;

public:
    virtual void saveEntities(App* app) = 0;
    virtual ~Saver() {}

};
