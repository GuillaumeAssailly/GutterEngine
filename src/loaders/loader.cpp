#include "loader.h"

void Loader::loadEntities(App* app) const {
    TransformComponent transform;
    RenderComponent render;
    PhysicsComponent physics;
    StaticPhysicsComponent SPhysics;
    LightComponent light;
    CameraComponent camera;

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
    loadBall(app, &camera);

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