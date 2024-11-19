#pragma once
#include "../config.h"

struct StaticPhysicsComponent {
    physx::PxRigidStatic* rigidBody;
};