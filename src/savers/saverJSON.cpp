#include "saverJSON.h"
#include <regex>

void SaverJSON::saveEntities()
{
	saveEntitiesNames();
	saveTransforms();
	saveRenders();
	saveCameras();
	saveLights();
	savePhysics();
    std::ofstream file(fileName);
    file << jsonData.dump(4); // Sauvegarde avec une indentation de 4 espaces
    file.close();

    std::cout << "Data saved to " << fileName << "\n";
}

SaverJSON::SaverJSON(std::string fileName, EntityManager *entityManager):
	fileName(fileName), Saver(entityManager)
{
    
}

void SaverJSON::saveTransforms()
{
	for (const auto& transformComponents : entityManager->getTransformComponents()) {
		json transform;
		transform["id"] = transformComponents.first;
		transform["name"] = entityManager->getEntityNames()[transformComponents.first];
		transform["component"] = transformComponents.second;
		jsonData["transforms"].push_back(transform);
	}

}

void SaverJSON::saveRenders()
{
	for (const auto& renderComponents : entityManager->getRenderComponents()) {
		json render;
		render["id"] = renderComponents.first;
		render["name"] = entityManager->getEntityNames()[renderComponents.first];
		for (const auto& renderComponent : renderComponents.second) {
            json j = renderComponent;
			render["component"].push_back(j);
		}
		/*render["id"] = renderComponents.first;
		render["name"] = entityManager.getEntityNames()[renderComponents.first];
		render["meshName"] = renderComponents.second.front().meshName;
		render["textureName"] = renderComponents.second.front().textureName;*/
		jsonData["renders"].push_back(render);
	}
}

void SaverJSON::saveCameras()
{
	json camera;
	for (const auto& cameraComponents : entityManager->getCameraComponents()) {
		camera["id"] = cameraComponents.first;
		camera["name"] = entityManager->getEntityNames()[cameraComponents.first];
		camera["component"] = cameraComponents.second;
		jsonData["cameras"].push_back(camera);
	}
}

void SaverJSON::saveLights()
{
	json light;
	for (const auto& lightComponents : entityManager->getLightComponents()) {
		light["id"] = lightComponents.first;
		light["name"] = entityManager->getEntityNames()[lightComponents.first];
		light["component"] = lightComponents.second;
		jsonData["lights"].push_back(light);
	}
}

void SaverJSON::saveEntitiesNames()
{
	for (const auto& entityNames : entityManager->getEntityNames()) {
		json entity;
		entity["id"] = entityNames.first;
		entity["name"] = entityNames.second;
		jsonData["entityNames"].push_back(entity);
	}
}

void SaverJSON::savePhysics()
{
    for (const auto& physcisComponent : entityManager->getPhysicsComponents()) {
        json physicsData;
        auto physics = physcisComponent.second;
		physicsData["id"] = physcisComponent.first;
		physicsData["name"] = entityManager->getEntityNames()[physcisComponent.first];
        physicsData["component"] = physics;
		jsonData["physics"].push_back(physicsData);
    }
}





