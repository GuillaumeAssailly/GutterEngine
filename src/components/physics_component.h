#pragma once
#include "../config.h"
#include "../loaders/json.hpp"
#include "../loaders/json_physx_utils.hpp"

struct PhysicsComponent {
    physx::PxRigidDynamic* rigidBody;
};

// fonction incomplete
inline void to_json(json& j, const PhysicsComponent& p) {
    if (p.rigidBody) {
        j = json{
            {"position", p.rigidBody->getGlobalPose()},
            {"velocity", p.rigidBody->getLinearVelocity()},
            {"mass", p.rigidBody->getMass()},
            {"isSleeping", p.rigidBody->isSleeping()}
        };
    }
    else {
        j = nullptr;
    }
}

// fonction incomplete
inline void from_json(const json& j, PhysicsComponent& p) {
    if (!j.is_null()) {
        physx::PxPhysics* physics = nullptr; // à compléter
		physx::PxScene* scene = nullptr; // à compléter

        physx::PxTransform transform = j.at("position").get<physx::PxTransform>();
        p.rigidBody = physics->createRigidDynamic(transform);

        p.rigidBody->setLinearVelocity(j.at("velocity").get<physx::PxVec3>());
        p.rigidBody->setMass(j.at("mass").get<float>());

        if (j.at("isSleeping").get<bool>()) {
            p.rigidBody->putToSleep();
        }
        else {
            p.rigidBody->wakeUp();
        }

        scene->addActor(*p.rigidBody);
    }
}