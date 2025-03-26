#pragma once
#include "../config.h"
#include "../components/transform_component.h"
#include "../components/physics_component.h"

class MotionSystem {
public:
    MotionSystem();
    ~MotionSystem();

    physx::PxRigidDynamic* createDynamic(const std::vector<physx::PxConvexMesh*>& convexMeshes, glm::vec3 material, glm::vec3 transform, float mass = 10.0f, float sleepT = 0.05f, float linearDamp = 0.0f, float angularDamp = 0.0f, int solverPosition = 4, int solverVelocity = 4, glm::quat rot={0,0,0,1.0f});
    physx::PxRigidDynamic* createDynamic(const physx::PxGeometry& geometry, glm::vec3 material, glm::vec3 transform, float mass, float sleepT = 0.05f, float linearDamp = 0.0f, float angularDamp = 0.0f, int solverPosition = 4, int solverVelocity = 4);
    physx::PxRigidStatic* createStatic(const std::vector<physx::PxTriangleMesh*>& convexMeshes, glm::vec3 material, glm::vec3 transform);
    physx::PxRigidStatic* createStatic(const physx::PxGeometry& geometry, glm::vec3 material, glm::vec3 transform);
    

    void concaveToConvex(const char* filePath, std::string outputDir, std::string baseName);
    void loadObjToPhysX(const std::string& filePath, std::vector<physx::PxConvexMesh*>& convexMeshes);
    void loadStaticObjToPhysX(const std::string& filePath, std::vector<physx::PxTriangleMesh*>& meshes);
    void applyForceToActor(physx::PxRigidDynamic* actor, const physx::PxVec3& force, const physx::PxForceMode::Enum mode = physx::PxForceMode::eIMPULSE);

    void update(
        std::unordered_map<unsigned int, TransformComponent>& transformComponents,
        std::unordered_map<unsigned int, PhysicsComponent>& physicsComponents,
        float dt);

private:
    int mNbThreads = 4;
    physx::PxPhysics* mPhysics;
    physx::PxPvd* mPvd;
    physx::PxScene* mScene;
    physx::PxDefaultCpuDispatcher* mCpuDispatcher;
    physx::PxFoundation* mFoundation;

};
