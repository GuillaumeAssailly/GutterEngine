#include "loaderJSON.h"

using json = nlohmann::json;

void LoaderJSON::loadQuilles(App* app, CameraComponent *camera) const {
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
    app->getMotionSystem()->loadObjToPhysX(jsonData["meshPath"].get<std::string>(), meshes);

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
        //PhysicsComponent physics;
        /*physics.rigidBody = app->motionSystem->createDynamic(
            meshes,
            pinMaterial,
            transform.position,
            rb["mass"].get<float>(),
            rb["staticFriction"].get<float>(),
            rb["dynamicFriction"].get<float>(),
            rb["restitution"].get<float>()
        );*/
        //app->physicsComponents[quille] = physics;

        std::tuple<unsigned int, unsigned int> model = app->make_model(pinData["modelPath"].get<std::string>().c_str());
        render.mesh = std::get<0>(model);
        render.indexCount = std::get<1>(model);
        app->renderComponents[quille] = render;
    }

    std::cout << "Quilles instaciees a partir du fichier " << fileName << " file.\n";

}

void LoaderJSON::loadEntities(App* app) const {
    TransformComponent transform;
    RenderComponent render;
    PhysicsComponent physics;
    StaticPhysicsComponent SPhysics;
    LightComponent light;
    CameraComponent camera;
    //Loader* loader = new LoaderRaw();

    // Lane
    unsigned int lane = app->make_entity("Lane");
    transform.position = { 0.f, 0.f, 9.f };
    transform.eulers = { 0.f, 0.f, 0.0f, 0.f };
    transform.size = { 1.0f, 0.168f, 18.0f };
    app->transformComponents[lane] = transform;

    glm::vec3 laneMaterial = { 0.05f, 0.05f, 0.0f };
    physx::PxBoxGeometry laneGeometry(physx::PxVec3(transform.size.x / 2.0f, transform.size.y / 2.0f, transform.size.z / 2.0f));

    SPhysics.rigidBody = app->getMotionSystem()->createStatic(laneGeometry, laneMaterial, transform.position);
    app->staticPhysicsComponents[lane] = SPhysics;

    render.mesh = app->getRenderModels()["Lane"].first;
    render.indexCount = app->getRenderModels()["Lane"].second;
    render.material = app->getTexturesList()["Lane"];
    app->renderComponents[lane] = render;

    // Ball
    unsigned int ball = app->make_entity("Ball");
    transform.position = { 0.0f, 0.105f, 3.0f };
    transform.eulers = { 0.0f, 0.0f, 0.0f , 0.f };
    app->transformComponents[ball] = transform;

    glm::vec3 ballMaterial = { 0.05f, 0.05f, 0.2f };
    const physx::PxSphereGeometry ballGeometry(0.105f);

    physics.rigidBody = app->getMotionSystem()->createDynamic(ballGeometry, ballMaterial, transform.position, 6.8f);
    app->physicsComponents[ball] = physics;

    std::tuple<unsigned int, unsigned int> ballModel = app->make_model("obj/servoskull/boule.obj");

    render.mesh = app->getRenderModels()["Ball"].first;
    render.indexCount = app->getRenderModels()["Ball"].second;
    render.material = app->getTexturesList()["Ball"];
    app->renderComponents[ball] = render;

    camera.fov = 60.0f;
    camera.aspectRatio = 16.0f / 9.0f;
    camera.nearPlane = 0.1f;
    camera.farPlane = 100.0f;
    camera.sensitivity = 0.1f;
    app->cameraComponents[ball] = camera;

    // Pins
    glm::vec3 first_pin = { 0.f, 0.22f, 15.f };
    loadQuilles(app, &camera);

    // Camera
    unsigned int cameraEntity = app->make_entity("Camera");
    transform.position = first_pin;
    transform.eulers = { 0.0f, 0.0f, 0.0f, 0.f };
    app->transformComponents[cameraEntity] = transform;

    camera.fov = 60.0f;
    camera.aspectRatio = 16.0f / 9.0f;
    camera.nearPlane = 0.1f;
    camera.farPlane = 100.0f;
    camera.sensitivity = 0.1f;
    app->cameraComponents[cameraEntity] = camera;
    app->cameraID = cameraEntity;

    //First light
    unsigned int lightEntity1 = app->make_entity("First Light");
    transform.position = first_pin;
    transform.eulers = { 0.0f, 0.0f, 0.0f, 0.f };
    app->transformComponents[lightEntity1] = transform;

    light.color = { 0.0f, 1.0f, 1.0f };
    light.intensity = 1.0f;
    app->lightComponents[lightEntity1] = light;

    render.mesh = app->getRenderModels()["Light"].first;
    render.indexCount = app->getRenderModels()["Light"].second;
    render.material = app->getTexturesList()["Light"];
    app->renderComponents[lightEntity1] = render;

    //Second light: 
    unsigned int lightEntity2 = app->make_entity("Second Light");
    transform.position = { 0.0f, 4.0f, 4.0f };
    transform.eulers = { 0.0f, 0.0f, 0.0f, 0.f };
    app->transformComponents[lightEntity2] = transform;

    light.color = { 1.0f, 1.0f, 1.0f };
    light.intensity = 1.0f;
    app->lightComponents[lightEntity2] = light;

    render.mesh = app->getRenderModels()["Light"].first;
    render.indexCount = app->getRenderModels()["Light"].second;
    render.material = app->getTexturesList()["Light"];
    app->renderComponents[lightEntity2] = render;
}

