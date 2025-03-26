#pragma once
#include "../config.h"
#include "../components/camera_component.h"
#include "../components/physics_component.h"
#include "../components/static_physics_component.h"
#include "../components/transform_component.h"
#include "../components/light_component.h"
#include "../components/render_component.h"
class App;
class MeshManager;
class Inputmanager;

class EntityManager
{
public:
    EntityManager();
    ~EntityManager();

    friend class App;
    friend class MeshManager;
    friend class InputManager;

    unsigned int make_entity(const std::string&);
    std::unordered_map<unsigned int, std::string>& getEntityNames();

    int cameraID = 0;

    glm::vec3 getPositionByName(std::string name);
    glm::vec3 getPositionByID(int id);
    void setPositionByName(std::string name, glm::vec3 position);
    void setPositionByID(int id, glm::vec3 position);

    glm::quat getRotationQuaternionByName(std::string name);
    glm::quat getRotationQuaternionByID(int id);
    void setRotationQuaternionByName(std::string name, glm::quat rotation);
    void setRotationQuaternionByID(int id, glm::quat rotation);
    glm::vec3 getRotationEulerByName(std::string name);
    glm::vec3 getRotationEulerByID(int id);
    void setRotationEulerByName(std::string name, glm::vec3 eulers);
    void setRotationEulerByID(int id, glm::vec3 eulers);

    std::vector<std::string> getAllCamerasNames();
    std::vector<int> getAllCamerasID();
    std::string getMainCameraName();
    int getMainCameraID();
    void setMainCameraByName(std::string name);
    void setMainCameraByID(int id);

    void setForceByName(std::string name, physx::PxVec3 force);
    void setForceById(int id, physx::PxVec3 force);
    physx::PxVec3 getLinearVelocityByName(std::string name);
    physx::PxVec3 getLinearVelocityById(int id);
    physx::PxVec3 getAngularVelocityByName(std::string name);
    physx::PxVec3 getAngularVelocityById(int id);
    void setLinearVelocityByName(std::string name, physx::PxVec3 velocity);
    void setLinearVelocityById(int id, physx::PxVec3 velocity);
    void setAngularVelocityByName(std::string name, physx::PxVec3 velocity);
    void setAngularVelocityById(int id, physx::PxVec3 velocity);
    void enablePhysicByName(std::string name);
    void enablePhysicById(int id);
    void disablePhysicByName(std::string name);
    void disablePhysicById(int id);

private:
    //Components
    std::unordered_map<unsigned int, std::string> entityNames;
    std::unordered_map<unsigned int, TransformComponent> transformComponents;
    std::unordered_map<unsigned int, PhysicsComponent> physicsComponents;
    std::unordered_map<unsigned int, StaticPhysicsComponent> staticPhysicsComponents;
    std::unordered_map<unsigned int, CameraComponent> cameraComponents;
    std::unordered_map<unsigned int, LightComponent> lightComponents;
    std::unordered_map<unsigned int, std::list<RenderComponent>> renderComponents;

    unsigned int entity_count = 0;
};