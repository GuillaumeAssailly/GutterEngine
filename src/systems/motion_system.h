#pragma once
#include "../config.h"
#include "../components/transform_component.h"
#include "../components/physics_component.h"

class MotionSystem {
public:
    MotionSystem();
    ~MotionSystem();

    void addRigidBody(btRigidBody* rigidBody);

    void update(
        std::unordered_map<unsigned int, TransformComponent>& transformComponents,
        std::unordered_map<unsigned int, PhysicsComponent>& physicsComponents,
        float dt);

private:
    btDiscreteDynamicsWorld* dynamicsWorld;
    btBroadphaseInterface* broadphase;
    btDefaultCollisionConfiguration* collisionConfiguration;
    btCollisionDispatcher* dispatcher;
    btSequentialImpulseConstraintSolver* solver;

};
