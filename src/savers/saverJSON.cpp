#include "saverJSON.h"
#include <regex>

void SaverJSON::savePins(App* app)
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
				jsonData["meshPath"] = "obj/convexMesh/quille.obj";
                const auto& camera = app->cameraComponents[id];
                jsonData["PinCamera"] = {
                    {"fov", camera.fov},
                    {"aspectRatio", camera.aspectRatio},
                    {"nearPlane", camera.nearPlane},
                    {"farPlane", camera.farPlane},
                    {"sensitivity", camera.sensitivity},
                    {"initialForward", {
                        {"x", 0},
                        {"y", 0},
                        {"z", 1},
                        {"w", 0}
                    }}
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
            //pin["modelPath"] = "obj/servoskull/quille.obj";
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


void SaverJSON::saveBall(App* app)
{
    unsigned int ballEntity = app->getEntityByName("Ball");
    const auto& camera = app->cameraComponents[ballEntity];
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
        jsonData["ball"]["camera"] = {
            {"fov", camera.fov},
            {"aspectRatio", camera.aspectRatio},
            {"nearPlane", camera.nearPlane},
            {"farPlane", camera.farPlane},
            {"sensitivity", camera.sensitivity},
            {"initialForward", {
                {"x", 0},
                {"y", 0},
                {"z", 1},
                {"w", 0}
            }}
        };
    }
}

void SaverJSON::saveLights(App* app)
{
    jsonData["lights"] = json::array();
    for (const auto& transformComponents : app->transformComponents) {
		int id = transformComponents.first;
        std::regex keyPattern(".*Light.*");
        if (std::regex_match(app->entityNames[transformComponents.first], keyPattern)) {
            const auto& transform = app->transformComponents.at(id);
			const auto& light = app->lightComponents.at(id);
            json lightData = {
                {"id", app->entityNames[transformComponents.first]},
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
                {"color", {
                    {"r", light.color.x},
                    {"g", light.color.y},
                    {"b", light.color.z}
                }},
                {"intensity", light.intensity},
                {"isDirectional", light.isDirectional},
                {"render", {
                    {"meshName", "Light"},
                    {"textureName", "Light"}
                }}
            };
            if (light.isDirectional) {
                lightData["direction"] = {
                    {"x", light.direction.x},
                    {"y", light.direction.y},
                    {"z", light.direction.z}
                };
            }
            jsonData["lights"].push_back(lightData);
        }
    }
}

void SaverJSON::saveCamera(App* app)
{
    unsigned int cameraEntity = app->getEntityByName("Camera");
    if (cameraEntity) {
        const auto& transform = app->transformComponents[cameraEntity];
        const auto& camera = app->cameraComponents[cameraEntity];
        jsonData["camera"] = {
            {"id", app->entityNames[cameraEntity]},
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
            {"fov", camera.fov},
            {"aspectRatio", camera.aspectRatio},
            {"nearPlane", camera.nearPlane},
            {"farPlane", camera.farPlane},
            {"sensitivity", camera.sensitivity},
            {"initialForward", {
                {"x", camera.initialForward.x},
                {"y", camera.initialForward.y},
                {"z", camera.initialForward.z},
                {"w", camera.initialForward.w}
            }}
        };
    }
}

void SaverJSON::saveLane(App* app)
{
    int laneEntity = -1;
    laneEntity = app->getEntityByName("Lane");
    if (laneEntity >= 0) {
        const auto& transform = app->transformComponents[laneEntity];
        const auto& staticPhysics = app->staticPhysicsComponents[laneEntity];
        jsonData["lane"] = {
            {"id", app->entityNames[laneEntity]},
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
            {"size", {
                {"x", transform.size.x},
                {"y", transform.size.y},
                {"z", transform.size.z}
            }},
            {"material", {
                {"r", 0.05}, {"g", 0.05}, {"b", 0.0}
            }},
            {"render", {
                {"meshName", "Lane"},
                {"textureName", "Lane"}
            }}
        };
    }
}

void SaverJSON::saveEntities(App* app)
{
	Saver::saveEntities(app);

    // Sauvegarde dans un fichier
    savePins(app);
    saveBall(app);
	saveLights(app);
	saveLane(app);
	saveCamera(app);
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

