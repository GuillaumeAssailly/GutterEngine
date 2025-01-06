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

void LoaderJSON::loadLight(App* app, CameraComponent* camera) const
{
    json lightsData = jsonData["lights"];
    TransformComponent transform;
    LightComponent light;
    RenderComponent render;
    for (const auto& lightData : lightsData) {
        // Créer une entité pour la lumière
        unsigned int lightEntity = app->make_entity(lightData["id"].get<std::string>());

        // Configurer la position et la rotation
        transform.position = glm::vec3(
            lightData["position"]["x"].get<float>(),
            lightData["position"]["y"].get<float>(),
            lightData["position"]["z"].get<float>()
        );
        transform.eulers = glm::vec3(
            lightData["rotation"]["x"].get<float>(),
            lightData["rotation"]["y"].get<float>(),
            lightData["rotation"]["z"].get<float>()
        );
        app->transformComponents[lightEntity] = transform;

        // Configurer la lumière (couleur et intensité)
        light.color = glm::vec3(
            lightData["color"]["r"].get<float>(),
            lightData["color"]["g"].get<float>(),
            lightData["color"]["b"].get<float>()
        );
        light.intensity = lightData["intensity"].get<float>();
        app->lightComponents[lightEntity] = light;

        // Charger le modèle pour le rendu
        std::string meshName = lightData["render"]["meshName"].get<std::string>();
        std::string textureName = lightData["render"]["textureName"].get<std::string>();
        render.mesh = app->getRenderModels()[meshName].first;
        render.indexCount = app->getRenderModels()[meshName].second;
        render.material = app->getTexturesList()[textureName];
        app->renderComponents[lightEntity] = render;

        std::cout << "Light instantiated: " << lightData["id"].get<std::string>() << "\n";
    }
}

void LoaderJSON::loadCamera(App* app, CameraComponent* camera) const
{
    json cameraData = jsonData["camera"];
    // Créer une entité pour la caméra
    unsigned int cameraEntity = app->make_entity(cameraData["id"].get<std::string>());

    // Configurer la position et la rotation
    TransformComponent transform;
    transform.position = glm::vec3(
        cameraData["position"]["x"].get<float>(),
        cameraData["position"]["y"].get<float>(),
        cameraData["position"]["z"].get<float>()
    );
    transform.eulers = glm::vec3(
        cameraData["rotation"]["x"].get<float>(),
        cameraData["rotation"]["y"].get<float>(),
        cameraData["rotation"]["z"].get<float>()
    );
    app->transformComponents[cameraEntity] = transform;

    // Configurer les paramètres de la caméra
    camera->fov = cameraData["fov"].get<float>();
    camera->aspectRatio = cameraData["aspectRatio"].get<float>();
    camera->nearPlane = cameraData["nearPlane"].get<float>();
    camera->farPlane = cameraData["farPlane"].get<float>();
    camera->sensitivity = cameraData["sensitivity"].get<float>();
    app->cameraComponents[cameraEntity] = *camera;

    // Assigner l'ID de la caméra principale
    app->cameraID = cameraEntity;

    std::cout << "Camera instantiated: " << cameraData["id"].get<std::string>() << "\n";
}

void LoaderJSON::loadLane(App* app, CameraComponent* camera) const
{
    json laneData = jsonData["lane"];
    // Créer une entité pour la lane
    unsigned int laneEntity = app->make_entity(laneData["id"].get<std::string>());

    // Configurer la position, la rotation et la taille
    TransformComponent transform;
    transform.position = glm::vec3(
        laneData["position"]["x"].get<float>(),
        laneData["position"]["y"].get<float>(),
        laneData["position"]["z"].get<float>()
    );
    transform.eulers = glm::vec3(
        laneData["rotation"]["x"].get<float>(),
        laneData["rotation"]["y"].get<float>(),
        laneData["rotation"]["z"].get<float>()
    );
    transform.size = glm::vec3(
        laneData["size"]["x"].get<float>(),
        laneData["size"]["y"].get<float>(),
        laneData["size"]["z"].get<float>()
    );
    app->transformComponents[laneEntity] = transform;

    // Matériau
    glm::vec3 laneMaterial(
        laneData["material"]["r"].get<float>(),
        laneData["material"]["g"].get<float>(),
        laneData["material"]["b"].get<float>()
    );

    // Géométrie
    physx::PxBoxGeometry laneGeometry(
        physx::PxVec3(
            transform.size.x / 2.0f,
            transform.size.y / 2.0f,
            transform.size.z / 2.0f
        )
    );

    // RigidBody statique
    StaticPhysicsComponent SPhysics;
    SPhysics.rigidBody = app->getMotionSystem()->createStatic(
        laneGeometry, laneMaterial, transform.position
    );
    app->staticPhysicsComponents[laneEntity] = SPhysics;

    // Modèle pour le rendu
    RenderComponent render;
    std::string meshName = laneData["render"]["meshName"].get<std::string>();
    std::string textureName = laneData["render"]["textureName"].get<std::string>();
    render.mesh = app->getRenderModels()[meshName].first;
    render.indexCount = app->getRenderModels()[meshName].second;
    render.material = app->getTexturesList()[textureName];
    app->renderComponents[laneEntity] = render;

    std::cout << "Lane instantiated: " << laneData["id"].get<std::string>() << "\n";
}

LoaderJSON::LoaderJSON()
{
    std::string fileName = "dataSave/quilles_test.json";

    std::ifstream file(fileName);
    file >> jsonData;

}



