#pragma once
#include "../config.h"
#include "../controllers/app.h"

class Loader {
    protected :
        EntityManager* entityManager;
		Loader(EntityManager* entityManager) : entityManager(entityManager) {}
        virtual void loadTransforms() = 0;
        virtual void loadRenders() = 0;
        virtual void loadCameras() = 0;
        virtual void loadLights() = 0;
        virtual void loadEntitiesNames() = 0;
        virtual void loadPhysics() = 0;
		//virtual void loadTransform(App* app, CameraComponent* camera) const = 0;
    public :
        virtual void loadEntities() = 0;
        virtual ~Loader() {}
};