#include "loaderJSON.h"

using json = nlohmann::json;

void LoaderJSON::loadQuilles(App* app) const {
    TransformComponent transform;
    PhysicsComponent physics;
    RenderComponent render;

    std::string fileName = "dataSave/quilles_test.json";

    std::ifstream file(fileName);
    json jsonData;
    file >> jsonData;

    glm::vec3 pinMaterial(
        jsonData["pinMaterial"]["r"].get<float>(),
        jsonData["pinMaterial"]["g"].get<float>(),
        jsonData["pinMaterial"]["b"].get<float>()
    );
    
    std::vector<physx::PxConvexMesh*> meshes;
    app->motionSystem->loadObjToPhysX(jsonData["meshPath"].get<std::string>(), meshes);

    // Parcourir toutes les quilles du fichier JSON
    for (const auto& pinData : jsonData["pins"]) {
        unsigned int quille = app->make_entity(pinData["id"].get<std::string>());

        transform.position = glm::vec3(
            pinData["position"]["x"].get<float>(),
            pinData["position"]["y"].get<float>(),
            pinData["position"]["z"].get<float>()
        );
        transform.eulers = glm::vec3(
            pinData["rotation"]["x"].get<float>(),
            pinData["rotation"]["y"].get<float>(),
            pinData["rotation"]["z"].get<float>()
        );
        app->transformComponents[quille] = transform;

        auto& rb = pinData["rigidBody"];
        PhysicsComponent physics;
        physics.rigidBody = app->motionSystem->createDynamic(
            meshes,
            pinMaterial,
            transform.position,
            rb["mass"].get<float>(),
            rb["staticFriction"].get<float>(),
            rb["dynamicFriction"].get<float>(),
            rb["restitution"].get<float>()
        );
        app->physicsComponents[quille] = physics;

        std::tuple<unsigned int, unsigned int> model = app->make_model(pinData["modelPath"].get<std::string>().c_str());
        render.mesh = std::get<0>(model);
        render.indexCount = std::get<1>(model);
        app->renderComponents[quille] = render;
    }

    std::cout << "Quilles instaciees a partir du fichier " << fileName << " file.\n";

}

