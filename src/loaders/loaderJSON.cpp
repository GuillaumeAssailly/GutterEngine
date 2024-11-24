#include "loaderJSON.h"

void LoaderJSON::loadQuilles(App* app, CameraComponent *camera) const {
    TransformComponent transform;
    PhysicsComponent physics;
    RenderComponent render;

    glm::vec3 pinMaterial(
        jsonData["pinMaterial"]["r"].get<float>(),
        jsonData["pinMaterial"]["g"].get<float>(),
        jsonData["pinMaterial"]["b"].get<float>()
    );
    
    std::vector<physx::PxConvexMesh*> meshes;
    app->getMotionSystem()->loadObjToPhysX(jsonData["meshPath"].get<std::string>(), meshes);

    // Parcourir toutes les quilles du fichier JSON
    for (const auto& pinData : jsonData["pins"]) {
        unsigned int pin = app->make_entity(pinData["id"].get<std::string>());

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
        app->transformComponents[pin] = transform;

        auto& rb = pinData["rigidBody"];
        physics.rigidBody = app->getMotionSystem()->createDynamic(
            app->getPhysicsModels()["Pin"],
            pinMaterial,
            transform.position,
            rb["mass"].get<float>(),
            rb["staticFriction"].get<float>(),
            rb["dynamicFriction"].get<float>(),
            rb["restitution"].get<float>(), 255, 255
        );
        app->physicsComponents[pin] = physics;

        render.mesh = app->getRenderModels()["Pin"].first;
        render.indexCount = app->getRenderModels()["Pin"].second;
        app->renderComponents[pin] = render;

        camera->fov = jsonData["PinCamera"]["fov"].get<float>();
        camera->aspectRatio = jsonData["PinCamera"]["aspectRatio"].get<float>();
        camera->nearPlane = jsonData["PinCamera"]["nearPlane"].get<float>();
        camera->farPlane = jsonData["PinCamera"]["farPlane"].get<float>();
        camera->sensitivity = jsonData["PinCamera"]["sensitivity"].get<float>();
        //camera.initialForward = { 0,0,-1,0 };
        app->cameraComponents[pin] = *camera;
    }

    std::cout << "Quilles instaciees a partir du fichier \n";

}

void LoaderJSON::loadBall(App* app, CameraComponent* camera) const{
    json ballData = jsonData["ball"];
    TransformComponent transform;
    PhysicsComponent physics;
    RenderComponent render;

    // Créer une entité pour la balle
    unsigned int ball = app->make_entity(ballData["id"].get<std::string>());

    // Position et rotation
    transform.position = glm::vec3(
        ballData["position"]["x"].get<float>(),
        ballData["position"]["y"].get<float>(),
        ballData["position"]["z"].get<float>()
    );
    transform.eulers = glm::vec3(
        ballData["rotation"]["x"].get<float>(),
        ballData["rotation"]["y"].get<float>(),
        ballData["rotation"]["z"].get<float>()
    );
    app->transformComponents[ball] = transform;

    // Matériau
    glm::vec3 ballMaterial(
        ballData["material"]["r"].get<float>(),
        ballData["material"]["g"].get<float>(),
        ballData["material"]["b"].get<float>()
    );

    // Géométrie
    const float radius = ballData["geometry"]["radius"].get<float>();
    physx::PxSphereGeometry ballGeometry(radius);

    // RigidBody
    const float mass = ballData["rigidBody"]["mass"].get<float>();
    physics.rigidBody = app->getMotionSystem()->createDynamic(ballGeometry, ballMaterial, transform.position, mass);
    app->physicsComponents[ball] = physics;

    // Modèle pour le rendu
    std::string modelPath = ballData["modelPath"].get<std::string>();
    std::tuple<unsigned int, unsigned int> ballModel = app->make_model(modelPath.c_str());

    render.mesh = std::get<0>(ballModel);
    render.indexCount = std::get<1>(ballModel);
    render.material = app->getTexturesList()[ballData["render"]["textureName"].get<std::string>()];
    app->renderComponents[ball] = render;

    // Paramètres de la caméra
    camera->fov = ballData["camera"]["fov"].get<float>();
    camera->aspectRatio = ballData["camera"]["aspectRatio"].get<float>();
    camera->nearPlane = ballData["camera"]["nearPlane"].get<float>();
    camera->farPlane = ballData["camera"]["farPlane"].get<float>();
    camera->sensitivity = ballData["camera"]["sensitivity"].get<float>();
    app->cameraComponents[ball] = *camera;

    std::cout << "Boule instaciee a partir du fichier \n";
}

LoaderJSON::LoaderJSON()
{
    std::string fileName = "dataSave/quilles_test.json";

    std::ifstream file(fileName);
    file >> jsonData;

}



