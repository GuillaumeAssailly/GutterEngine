#pragma once
#include "json.hpp"
#include <PxPhysicsAPI.h>

using json = nlohmann::json;

namespace physx {

    inline void to_json(json& j, const physx::PxVec3& v) {
        j = json::array({ v.x, v.y, v.z });
    }

    inline void from_json(const json& j, physx::PxVec3& v) {
        v.x = j.at(0).get<float>();
        v.y = j.at(1).get<float>();
        v.z = j.at(2).get<float>();
    }

    inline void to_json(json& j, const physx::PxTransform& t) {
        j = json{
            {"position", t.p},  // Utilise `PxVec3`
            {"rotation", {t.q.w, t.q.x, t.q.y, t.q.z}} // Utilise `PxQuat`
        };
    }

    inline void from_json(const json& j, physx::PxTransform& t) {
        t.p = j.at("position").get<physx::PxVec3>();
        t.q.w = j.at("rotation")[0].get<float>();
        t.q.x = j.at("rotation")[1].get<float>();
        t.q.y = j.at("rotation")[2].get<float>();
        t.q.z = j.at("rotation")[3].get<float>();
    }
}
