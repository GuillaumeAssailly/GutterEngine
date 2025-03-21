#include "entity_manager.h"

EntityManager::EntityManager() : entity_count(0)
{

}

EntityManager::~EntityManager()
{
}

unsigned int EntityManager::make_entity(const std::string& name) {
	entityNames.insert(std::make_pair(entity_count, name));
	return entity_count++;
}

std::unordered_map<unsigned int, std::string>& EntityManager::getEntityNames() {
	return entityNames;
}

std::unordered_map<unsigned int, TransformComponent>& EntityManager::getTransformComponents()
{
    return transformComponents;
}

std::unordered_map<unsigned int, std::list<RenderComponent>>& EntityManager::getRenderComponents()
{
    return renderComponents;
}

std::unordered_map<unsigned int, CameraComponent>& EntityManager::getCameraComponents()
{
	return cameraComponents;
}

std::unordered_map<unsigned int, LightComponent>& EntityManager::getLightComponents()
{
	return lightComponents;
}

std::unordered_map<unsigned int, PhysicsComponent>& EntityManager::getPhysicsComponents()
{
	return physicsComponents;
}



void EntityManager::renameEntity(unsigned int entity, std::string newName)
{
    bool isUnique = true;
    for (const auto& [key, value] : entityNames) {
        if (value == newName) {
            isUnique = false;
            break;
        }
    }
    if (isUnique && !newName.empty()) {
        entityNames[entity] = newName;
    }

}

std::string EntityManager::removeEntity(unsigned int entity)
{
    if (entity != cameraID) {
        entityNames.erase(entity);
        transformComponents.erase(entity);
        physicsComponents.erase(entity);
        staticPhysicsComponents.erase(entity);
        cameraComponents.erase(entity);
        lightComponents.erase(entity);
        renderComponents.erase(entity);
        return "";
    }
    return "You can't delete the current camera";
}

int EntityManager::getEntityCount() {
    return entity_count; 
}
void EntityManager::addEntityCount() {
    entity_count++;
}