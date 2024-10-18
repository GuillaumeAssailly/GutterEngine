#include "motion_system.h"

void MotionSystem::update(
    std::unordered_map<unsigned int, TransformComponent>& transformComponents,
    std::unordered_map<unsigned int, PhysicsComponent>& physicsComponents,
    float dt) {

    //const float gravity = -9.81f;
    for (std::pair<unsigned int, PhysicsComponent> entity : physicsComponents) {
        PhysicsComponent& physics = entity.second;
        TransformComponent& transform = transformComponents[entity.first];

        //physics.velocity.y += gravity * dt;

        transform.position += physics.velocity * dt;

        transform.eulers += physics.eulerVelocity * dt;

    }
}