#pragma once

#include "../loaders/json.hpp"
#include "saver.h"

using json = nlohmann::json;

class SaverJSON : public Saver
{
public:
    void saveEntities() override;
    SaverJSON(std::string, EntityManager *entityManager);

private:
    std::string fileName;
	EntityManager *entityManager;
    json jsonData;
	void saveTransforms() override;
	void saveRenders() override;
	void saveCameras() override;
	void saveLights() override;
	void saveEntitiesNames() override;
	void savePhysics() override;
    

};
