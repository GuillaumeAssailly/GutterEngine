#pragma once
#include "../config.h"
#include "../components/transform_component.h"
#include "../components/physics_component.h"

class MotionSystem {
public:
    MotionSystem();
    ~MotionSystem();

    physx::PxRigidDynamic* createDynamic(const std::vector<physx::PxConvexMesh*>& convexMeshes, glm::vec3 material, glm::vec3 transform, float mass, float sleepT = 0.05f, float linearDamp = 0.0f, float angularDamp = 0.0f);
    physx::PxRigidDynamic* createDynamic(const physx::PxGeometry& geometry, glm::vec3 material, glm::vec3 transform, float mass, float sleepT = 0.05f, float linearDamp = 0.0f, float angularDamp = 0.0f);
    void createStatic(const physx::PxGeometry& geometry, glm::vec3 material, glm::vec3 transform);
    physx::PxConvexMesh* createMesh(std::vector<physx::PxVec3> vertices);
    void applyForceToActor(physx::PxRigidDynamic* actor, const physx::PxVec3& force, const physx::PxForceMode::Enum mode = physx::PxForceMode::eIMPULSE);
    void loadObjToPhysX(const std::string& filePath, std::vector<physx::PxConvexMesh*>& convexMeshes);

    void update(
        std::unordered_map<unsigned int, TransformComponent>& transformComponents,
        std::unordered_map<unsigned int, PhysicsComponent>& physicsComponents,
        float dt);

private:
    int mNbThreads = 4;
    physx::PxPhysics* mPhysics; // Attribut pour garder une référence à l'objet PhysX
    physx::PxPvd* mPvd;
    physx::PxScene* mScene;     // Scène PhysX
    physx::PxDefaultCpuDispatcher* mCpuDispatcher; // Gestionnaire de tâches
    physx::PxFoundation* mFoundation;

};
