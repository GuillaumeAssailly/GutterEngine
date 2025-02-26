#pragma once

#include "../loaders/json.hpp"
#include "saver.h"

using json = nlohmann::json;

class SaverJSON : public Saver
{
public:
    void saveEntities(App* app) override;
    SaverJSON(std::string);

private:
    std::string fileName;
    json jsonData;
    //void savePins(App* app) override;
    //void savePinMaterial(App* app, int pinId);
    /*void saveBall(App* app) override;
    void saveLights(App* app) override;
    void saveCamera(App* app) override;
    void saveLane(App* app) override;*/
	void saveTransforms(EntityManager entityManager) override;
	void saveRenders(EntityManager entityManager) override;
	void saveCameras(EntityManager entityManager) override;
	void saveLights(EntityManager entityManager) override;
	void saveEntitiesNames(EntityManager entityManager) override;
	void savePhysics(EntityManager entityManager) override;
    

};
