#include "motion_system.h"

MotionSystem::MotionSystem() {
    // Initialize Bullet Physics
    broadphase = new btDbvtBroadphase();
    collisionConfiguration = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfiguration);
    solver = new btSequentialImpulseConstraintSolver();
    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
    dynamicsWorld->setGravity(btVector3(0, -9.81, 0));
}

MotionSystem::~MotionSystem() {
    delete dynamicsWorld;
    delete solver;
    delete dispatcher;
    delete collisionConfiguration;
    delete broadphase;
}

void MotionSystem::addRigidBody(btRigidBody* rigidBody)
{
    btTransform bulletTransform;
    dynamicsWorld->addRigidBody(rigidBody);
    rigidBody->getMotionState()->getWorldTransform(bulletTransform);
    btVector3 position = bulletTransform.getOrigin();
    printf("Pos : %f, %f, %f\n", position.getX(), position.getY(), position.getZ());
}


void MotionSystem::update(
    std::unordered_map<unsigned int, TransformComponent>& transformComponents,
    std::unordered_map<unsigned int, PhysicsComponent>& physicsComponents,
    float dt) {

    dynamicsWorld->stepSimulation(dt, 10);

    //printf("%f | %f | %f\n\n", transformComponents[2].position.x, transformComponents[2].position.y, transformComponents[2].position.z);

    for (auto& entity : physicsComponents) {
        PhysicsComponent& physicsComponent = entity.second;
        TransformComponent& transform = transformComponents[entity.first];

        btTransform bulletTransform;
        physicsComponent.rigidBody->getMotionState()->getWorldTransform(bulletTransform);

        btVector3 position = bulletTransform.getOrigin();
        btQuaternion rotation = bulletTransform.getRotation();

        transform.position = glm::vec3(position.getX(), position.getY(), position.getZ());
        //printf("Pos : %f, %f, %f\n", position.getX(), position.getY(), position.getZ());
        transform.eulers = glm::eulerAngles(glm::quat(rotation.getW(), rotation.getX(), rotation.getY(), rotation.getZ()));

        if(entity.first == 2)
            printf("Eulers : %f, %f, %f | %f %f %f %f\n", transform.eulers.x, transform.eulers.y, transform.eulers.z, rotation.getW(), rotation.getX(), rotation.getY(), rotation.getZ());

    }
}