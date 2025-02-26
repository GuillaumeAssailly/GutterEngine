#include "saverJSON.h"
#include <regex>
/*
void SaverJSON::savePins(App* app)
{
    int countPins = 0;
    for (const auto& transformComponents : entityManager->transformComponents) {
        if (entityManager->entityNames[transformComponents.first].find("Pin") == 0) {
            countPins++;
            json pin;
            int id = transformComponents.first;
            auto physics = entityManager->physicsComponents.at(id);
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
                const auto& camera = entityManager->cameraComponents[id];
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
            pin["id"] = entityManager->entityNames[transformComponents.first];
            pin["position"] = {
                {"x", transformComponents.second.position.x},
                {"y", transformComponents.second.position.y},
                {"z", transformComponents.second.position.z}
            };
            pin["rotation"] = {
                {"x", transformComponents.second.eulers.x},
                {"y", transformComponents.second.eulers.y},
                {"z", transformComponents.second.eulers.z},
                {"w", transformComponents.second.eulers.w }
            };
            //pin["modelPath"] = "obj/servoskull/quille.obj";
            jsonData["pins"].push_back(pin);
        }
    }
}

void SaverJSON::saveBall(App* app)
{
    unsigned int ballEntity = entityManager->getEntityByName("Ball");
    const auto& camera = entityManager->cameraComponents[ballEntity];
    if (ballEntity) {
        const auto& transform = entityManager->transformComponents[ballEntity];
        const auto& physics = entityManager->physicsComponents[ballEntity];
        jsonData["ball"] = {
            {"id", entityManager->entityNames[ballEntity]},
            {"position", {
                {"x", transform.position.x},
                {"y", transform.position.y},
                {"z", transform.position.z},
            }},
            {"rotation", {
                {"x", transform.eulers.x},
                {"y", transform.eulers.y},
				{"z", transform.eulers.z},
				{"w", transform.eulers.w}
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
    for (const auto& transformComponents : entityManager->transformComponents) {
		int id = transformComponents.first;
        std::regex keyPattern(".*Light.*");
        if (std::regex_match(entityManager->entityNames[transformComponents.first], keyPattern)) {
            const auto& transform = entityManager->transformComponents.at(id);
			const auto& light = entityManager->lightComponents.at(id);
            json lightData = {
                {"id", entityManager->entityNames[transformComponents.first]},
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
                {"isDirectional", light.type},
                {"render", {
                    {"meshName", "Light"},
                    {"textureName", "Light"}
                }}
            };
            if (light.type == DIRECTIONAL) {
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
    unsigned int cameraEntity = entityManager->getEntityByName("Camera");
    if (cameraEntity) {
        const auto& transform = entityManager->transformComponents[cameraEntity];
        const auto& camera = entityManager->cameraComponents[cameraEntity];
        jsonData["camera"] = {
            {"id", entityManager->entityNames[cameraEntity]},
            {"position", {
                {"x", transform.position.x},
                {"y", transform.position.y},
                {"z", transform.position.z}
            }},
            {"rotation", {
                {"x", transform.eulers.x},
                {"y", transform.eulers.y},
                {"z", transform.eulers.z},
                {"w", transform.eulers.w}
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
    laneEntity = entityManager->getEntityByName("Lane");
    if (laneEntity >= 0) {
        const auto& transform = entityManager->transformComponents[laneEntity];
        const auto& staticPhysics = entityManager->staticPhysicsComponents[laneEntity];
        jsonData["lane"] = {
            {"id", entityManager->entityNames[laneEntity]},
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
}*/

void SaverJSON::saveEntities(App* app)
{
	//Saver::saveEntities(app);

    // Sauvegarde dans un fichier
    //savePins(app);
    //saveBall(app);
	//saveLights(app);
	//saveLane(app);
	//saveCamera(app);
	saveEntitiesNames(*(app->getEntityManager()));
	saveTransforms(*(app->getEntityManager()));
	saveRenders(*(app->getEntityManager()));
	saveCameras(*(app->getEntityManager()));
	saveLights(*(app->getEntityManager()));
	savePhysics(*(app->getEntityManager()));
    std::ofstream file(fileName);
    file << jsonData.dump(4); // Sauvegarde avec une indentation de 4 espaces
    file.close();

    std::cout << "Data saved to " << fileName << "\n";
}

SaverJSON::SaverJSON(std::string fileName):
    fileName(fileName)
{
    
}

void SaverJSON::saveTransforms(EntityManager entityManager)
{
	for (const auto& transformComponents : entityManager.getTransformComponents()) {
		json transform;
		transform["id"] = transformComponents.first;
		transform["name"] = entityManager.getEntityNames()[transformComponents.first];
		transform["component"] = transformComponents.second;
		jsonData["transforms"].push_back(transform);
	}

}

void SaverJSON::saveRenders(EntityManager entityManager)
{
	for (const auto& renderComponents : entityManager.getRenderComponents()) {
		json render;
		render["id"] = renderComponents.first;
		render["name"] = entityManager.getEntityNames()[renderComponents.first];
		for (const auto& renderComponent : renderComponents.second) {
            json j = renderComponent;
			render["component"].push_back(j);
		}
		/*render["id"] = renderComponents.first;
		render["name"] = entityManager.getEntityNames()[renderComponents.first];
		render["meshName"] = renderComponents.second.front().meshName;
		render["textureName"] = renderComponents.second.front().textureName;*/
		jsonData["renders"].push_back(render);
	}
}

void SaverJSON::saveCameras(EntityManager entityManager)
{
	json camera;
	for (const auto& cameraComponents : entityManager.getCameraComponents()) {
		camera["id"] = cameraComponents.first;
		camera["name"] = entityManager.getEntityNames()[cameraComponents.first];
		camera["component"] = cameraComponents.second;
		jsonData["cameras"].push_back(camera);
	}
}

void SaverJSON::saveLights(EntityManager entityManager)
{
	json light;
	for (const auto& lightComponents : entityManager.getLightComponents()) {
		light["id"] = lightComponents.first;
		light["name"] = entityManager.getEntityNames()[lightComponents.first];
		light["component"] = lightComponents.second;
		jsonData["lights"].push_back(light);
	}
}

void SaverJSON::saveEntitiesNames(EntityManager entityManager)
{
	for (const auto& entityNames : entityManager.getEntityNames()) {
		json entity;
		entity["id"] = entityNames.first;
		entity["name"] = entityNames.second;
		jsonData["entityNames"].push_back(entity);
	}
}

void SaverJSON::savePhysics(EntityManager entityManager)
{
    for (const auto& physcisComponent : entityManager.getPhysicsComponents()) {
        json physicsData;
        auto physics = physcisComponent.second;
		physicsData["id"] = physcisComponent.first;
		physicsData["name"] = entityManager.getEntityNames()[physcisComponent.first];
        physicsData["component"] = physics;
		jsonData["physics"].push_back(physicsData);
    }
}





