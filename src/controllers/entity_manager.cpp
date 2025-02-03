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

// Getters
std::unordered_map<unsigned int, std::string>& EntityManager::getEntityNames() {
    return entityNames;
}

std::unordered_map<unsigned int, TransformComponent>& EntityManager::getTransformComponents() {
    return transformComponents;
}

std::unordered_map<unsigned int, PhysicsComponent>& EntityManager::getPhysicsComponents() {
    return physicsComponents;
}

std::unordered_map<unsigned int, StaticPhysicsComponent>& EntityManager::getStaticPhysicsComponents() {
    return staticPhysicsComponents;
}

std::unordered_map<unsigned int, CameraComponent>& EntityManager::getCameraComponents() {
    return cameraComponents;
}

std::unordered_map<unsigned int, LightComponent>& EntityManager::getLightComponents() {
    return lightComponents;
}

std::unordered_map<unsigned int, std::list<RenderComponent>>& EntityManager::getRenderComponents() {
    return renderComponents;
}

unsigned int EntityManager::getEntityCount() const {
    return entity_count;
}