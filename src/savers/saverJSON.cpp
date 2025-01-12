#include "saverJSON.h"

void SaverJSON::saveQuilles(App* app, CameraComponent* camera)
{
    int countPins = 0;
    for (const auto& transformComponents : app->transformComponents) {
        if (app->entityNames[transformComponents.first].find("Pin") == 0) {
            countPins++;
            json pin;
            int id = transformComponents.first;
            auto physics = app->physicsComponents.at(id);
            if (countPins == 1) {
                physx::PxU32 solverPosition;
                physx::PxU32 solverVelocity;
                
                physics.rigidBody->getSolverIterationCounts(solverPosition, solverVelocity);
                jsonData["PinRigidBody"] = {
                    {"mass", physics.rigidBody->getMass()},
                    {"staticFriction", physics.rigidBody->getSleepThreshold()},
                    {"dynamicFriction", physics.rigidBody->getLinearDamping()},
                    {"restitution", physics.rigidBody->getAngularDamping()},
                    {"solverPosition", solverPosition},
                    {"solverVelocity", solverVelocity}
                };
                savePinMaterial(app, id);
            }
            pin["id"] = app->entityNames[transformComponents.first];
            pin["position"] = {
                {"x", transformComponents.second.position.x},
                {"y", transformComponents.second.position.y},
                {"z", transformComponents.second.position.z}
            };
            pin["rotation"] = {
                {"x", transformComponents.second.eulers.x},
                {"y", transformComponents.second.eulers.y},
                {"z", transformComponents.second.eulers.z}
            };
            pin["modelPath"] = "obj/servoskull/quille.obj";
            jsonData["pins"].push_back(pin);
        }
    }
}

void SaverJSON::savePinMaterial(App* app, int pinId)
{
    auto physics = app->physicsComponents.at(pinId);

    const int maxShapes = 10;
    physx::PxShape* shapes[maxShapes];
    int shapeCount = physics.rigidBody->getNbShapes();
    physics.rigidBody->getShapes(shapes, shapeCount);

    if (shapeCount > 0) {
        physx::PxMaterial* material;
        shapes[0]->getMaterials(&material, 1);
        jsonData["PinMaterial"] = {
            {"r", material->getStaticFriction()},
            {"g", material->getDynamicFriction()},
            {"b", material->getRestitution()},
        };
    }
}


void SaverJSON::saveBall(App* app, CameraComponent* camera)
{
    unsigned int ballEntity = app->getEntityByName("Ball");
    if (ballEntity) {
        const auto& transform = app->transformComponents[ballEntity];
        const auto& physics = app->physicsComponents[ballEntity];
        jsonData["ball"] = {
            {"id", app->entityNames[ballEntity]},
            {"position", {
                {"x", transform.position.x},
                {"y", transform.position.y},
                {"z", transform.position.z}
            }},
            {"rotation", {
                {"x", transform.eulers.x},
                {"y", transform.eulers.y},
                {"z", transform.eulers.z}
            }},
            {"material", {
                {"r", 0.05}, {"g", 0.05}, {"b", 0.2}
            }},
            {"geometry", {
                {"type", "sphere"},
                {"radius", 0.105}
            }},
            {"rigidBody", {
                {"mass", physics.rigidBody->getMass()}
            }},
            {"modelPath", "obj/servoskull/boule.obj"},
            {"render", {
                {"meshName", "Ball"},
                {"textureName", "Ball"}
            }}
        };
    }
}

void SaverJSON::saveLight(App* app, CameraComponent* camera)
{
}

void SaverJSON::saveCamera(App* app, CameraComponent* camera)
{
}

void SaverJSON::saveLane(App* app, CameraComponent* camera)
{
}

void SaverJSON::saveEntities(App* app)
{
	Saver::saveEntities(app);

    // Sauvegarde dans un fichier
    saveQuilles(app, NULL);
    saveBall(app, NULL);
    std::ofstream file(fileName);
    file << jsonData.dump(4); // Sauvegarde avec une indentation de 4 espaces
    file.close();

    std::cout << "Data saved to " << fileName << "\n";
}

SaverJSON::SaverJSON(std::string fileName):
    fileName(fileName)
{
    jsonData["pins"] = json::array();
    jsonData["ball"] = json({});
    jsonData["lights"] = json::array();
    jsonData["camera"] = json({});
    jsonData["lane"] = json({});


}

