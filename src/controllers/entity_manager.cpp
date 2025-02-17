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


// Position

glm::vec3 EntityManager::getPositionByName(std::string name) {
    for (auto const& entity : entityNames)
    {
        if (entity.second == name) {
            return getPositionByID(entity.first);
        }
    }
    throw std::out_of_range("Error getPositionByName : Entity with the given name " + name + " not found.");
}

glm::vec3 EntityManager::getPositionByID(int id) {
    if (transformComponents.find(id) != transformComponents.end())
        return transformComponents[id].position;
    else
        throw std::out_of_range("Error getPositionByID : Entity with the given ID " + std::to_string(id) + " not found.");
}

void EntityManager::setPositionByName(std::string name, glm::vec3 position) {
    for (auto const& entity : entityNames)
    {
        if (entity.second == name) {
            return setPositionByID(entity.first, position);
        }
    }
    throw std::out_of_range("Error setPositionByName : Entity with the given name " + name + " not found.");
}

void EntityManager::setPositionByID(int id, glm::vec3 position) {
    if (transformComponents.find(id) != transformComponents.end()) {
        transformComponents[id].position = position;

        // PhysX position
        if (physicsComponents.find(id) != physicsComponents.end()) {
            physx::PxTransform transform = physicsComponents[id].rigidBody->getGlobalPose();
            transform.p.x = position.x;
            transform.p.y = position.y;
            transform.p.z = position.z;
            physicsComponents[id].rigidBody->setGlobalPose(transform);
        }
        else if (staticPhysicsComponents.find(id) != staticPhysicsComponents.end()) {
            physx::PxTransform transform = staticPhysicsComponents[id].rigidBody->getGlobalPose();
            transform.p.x = position.x;
            transform.p.y = position.y;
            transform.p.z = position.z;
            staticPhysicsComponents[id].rigidBody->setGlobalPose(transform);
        }
    }
    else
        throw std::out_of_range("Error setPositionByID : Entity with the given ID " + std::to_string(id) + " not found.");
}

// Rotation

glm::quat EntityManager::getRotationQuaternionByName(std::string name) {
    for (auto const& entity : entityNames) {
        if (entity.second == name) {
            return getRotationQuaternionByID(entity.first);
        }
    }
    throw std::out_of_range("Error getRotationQuaternionByName : Entity with the given name " + name + " not found.");
}

glm::quat EntityManager::getRotationQuaternionByID(int id) {
    if (transformComponents.find(id) != transformComponents.end())
        return transformComponents[id].eulers; // Assuming eulers is a quaternion
    else
        throw std::out_of_range("Error getRotationQuaternionByID : Entity with the given ID " + std::to_string(id) + " not found.");
}

void EntityManager::setRotationQuaternionByName(std::string name, glm::quat rotation) {
    for (auto const& entity : entityNames) {
        if (entity.second == name) {
            return setRotationQuaternionByID(entity.first, rotation);
        }
    }
    throw std::out_of_range("Error setRotationQuaternionByName : Entity with the given name " + name + " not found.");
}

void EntityManager::setRotationQuaternionByID(int id, glm::quat rotation) {
    if (transformComponents.find(id) != transformComponents.end()) {
        transformComponents[id].eulers = rotation;

        // PhysX rotation
        if (physicsComponents.find(id) != physicsComponents.end()) {
            physx::PxTransform transform = physicsComponents[id].rigidBody->getGlobalPose();
            transform.q.w = rotation.w;
            transform.q.x = rotation.x;
            transform.q.y = rotation.y;
            transform.q.z = rotation.z;
            physicsComponents[id].rigidBody->setGlobalPose(transform);
        }
        else if (staticPhysicsComponents.find(id) != staticPhysicsComponents.end()) {
            physx::PxTransform transform = staticPhysicsComponents[id].rigidBody->getGlobalPose();
            transform.q.w = rotation.w;
            transform.q.x = rotation.x;
            transform.q.y = rotation.y;
            transform.q.z = rotation.z;
            staticPhysicsComponents[id].rigidBody->setGlobalPose(transform);
        }
    }
    else
        throw std::out_of_range("Error setRotationQuaternionByID : Entity with the given ID " + std::to_string(id) + " not found.");
}

glm::vec3 EntityManager::getRotationEulerByName(std::string name) {
    return glm::eulerAngles(getRotationQuaternionByName(name));
}

glm::vec3 EntityManager::getRotationEulerByID(int id) {
    return glm::eulerAngles(getRotationQuaternionByID(id));
}

void EntityManager::setRotationEulerByName(std::string name, glm::vec3 eulers) {
    setRotationQuaternionByName(name, glm::quat(eulers));
}

void EntityManager::setRotationEulerByID(int id, glm::vec3 eulers) {
    setRotationQuaternionByID(id, glm::quat(eulers));
}

// Camera

std::vector<std::string> EntityManager::getAllCamerasNames() {
    std::vector<int> vectID = getAllCamerasID();
    std::vector<std::string> vectNames;
    for (auto id : vectID) {
        vectNames.push_back(entityNames[id]);
    }
    return vectNames;
}

std::vector<int> EntityManager::getAllCamerasID() {
    std::vector<int> vect;
    for (auto& camera : cameraComponents) {
        vect.push_back(camera.first);
    }
    return vect;
}

std::string EntityManager::getMainCameraName() {
    return entityNames[cameraID];
}

int EntityManager::getMainCameraID() {
    return cameraID;
}

void EntityManager::setMainCameraByName(std::string name) {
    for (auto const& entity : entityNames)
    {
        if (entity.second == name) {
            return setMainCameraByID(entity.first);
        }
    }
    throw std::out_of_range("Error setMainCameraByName : Camera with the given name " + name + " not found.");
}

void EntityManager::setMainCameraByID(int id) {
    if (cameraComponents.find(id) != cameraComponents.end())
        cameraID = id;
    else
        throw std::out_of_range("Error setMainCameraByID : Camera with the given ID " + std::to_string(id) + " not found.");
}