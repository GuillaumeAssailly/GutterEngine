#include "loaderJSON.h"

LoaderJSON::LoaderJSON(std::string fileName, EntityManager* entityManager):
	Loader::Loader(entityManager), fileName(fileName)
{
	std::ifstream file(fileName);
	file >> jsonData;
}

void LoaderJSON::loadTransforms()
{
	for (const auto& transformData : jsonData["transforms"]) {
		TransformComponent transformComponent = transformData["component"];
		entityManager->getTransformComponents().insert(std::make_pair(transformData["id"].get<int>(), transformComponent));
	}

}

void LoaderJSON::loadRenders()
{
	for (const auto& renderData : jsonData["renders"]) {
		std::list<RenderComponent> renderComponents;
		for (const auto& renderComponentData : renderData["component"]) {
			RenderComponent renderComponent = renderComponentData;
			renderComponents.push_back(renderComponent);
		}
		entityManager->getRenderComponents().insert(std::make_pair(renderData["id"].get<int>(), renderComponents));
	}
}

void LoaderJSON::loadCameras()
{
}

void LoaderJSON::loadLights()
{
}

void LoaderJSON::loadEntitiesNames()
{
	for (const auto& entityNamesData : jsonData["entityNames"]) {
		if (entityNamesData["id"].get<int>() > 2) {
			entityManager->addEntityCount();
		}
		entityManager->getEntityNames().insert(std::make_pair<int, std::string>(
			entityNamesData["id"].get<int>(), entityNamesData["name"].get<std::string>()));
	}

}

void LoaderJSON::loadPhysics()
{
}

void LoaderJSON::loadEntities()
{
	loadEntitiesNames();
	loadTransforms();
	loadRenders();
	loadCameras();
	loadLights();
	loadPhysics();
}

