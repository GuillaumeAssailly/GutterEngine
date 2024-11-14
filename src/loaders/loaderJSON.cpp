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

        physics.velocity = { 0.0f, 0.0f, 0.0f };
        physics.eulerVelocity = { 0.0f, 0.0f, 0.0f };
        app->physicsComponents[quille] = physics;

        auto model = app->make_model("obj/servoskull/quille.obj");
        render.mesh = std::get<0>(model);
        render.indexCount = std::get<1>(model);
        app->renderComponents[quille] = render;
    }

    std::cout << "Quilles instaciées à partir du fichier " << fileName << " file.\n";

}

