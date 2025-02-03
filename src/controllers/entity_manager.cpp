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