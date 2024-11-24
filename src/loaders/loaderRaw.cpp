#include "loaderRaw.h"

void LoaderRaw::loadQuilles(App *app, CameraComponent *camera) const
{
	TransformComponent transform;
	PhysicsComponent physics;
	RenderComponent render;
	std::vector<physx::PxConvexMesh*> meshes;
	glm::vec3 pinMaterial = { 0.5f, 0.5f, 0.5f };

	glm::vec3 first_pin = { 0.f, 0.22f, 15.f };
	glm::vec3 vectors[10];
	vectors[0] = first_pin;
	vectors[1] = glm::vec3(first_pin.x + 0.1524f, first_pin.y, first_pin.z + 0.2635f);
	vectors[2] = glm::vec3(first_pin.x - 0.1524f, first_pin.y, first_pin.z + 0.2635f);
	vectors[3] = glm::vec3(first_pin.x - 0.3048f, first_pin.y, first_pin.z + 0.527f);
	vectors[4] = glm::vec3(first_pin.x, first_pin.y, first_pin.z + 0.527f);
	vectors[5] = glm::vec3(first_pin.x + 0.3048f, first_pin.y, first_pin.z + 0.527f);
	vectors[6] = glm::vec3(first_pin.x + 0.1524f, first_pin.y, first_pin.z + 0.7905f);
	vectors[7] = glm::vec3(first_pin.x - 0.1524f, first_pin.y, first_pin.z + 0.7905f);
	vectors[8] = glm::vec3(first_pin.x + 0.4572f, first_pin.y, first_pin.z + 0.7905f);
	vectors[9] = glm::vec3(first_pin.x - 0.4572f, first_pin.y, first_pin.z + 0.7905f);

    for (int i = 0; i < 10; i++) {
        unsigned int pin = app->make_entity("Pin " + std::to_string(i));
        transform.position = vectors[i];
        transform.eulers = { 0.0f, 0.0f, 0.0f, 0.f };
        app->transformComponents[pin] = transform;

        physics.rigidBody = app->getMotionSystem()->createDynamic(app->getPhysicsModels()["Pin"], 
            pinMaterial, transform.position, 1.5f, 0.01f, 0.2f, 0.3f, 255, 255);
        app->physicsComponents[pin] = physics;

        render.mesh = app->getRenderModels()["Pin"].first;
        render.indexCount = app->getRenderModels()["Pin"].second;
        app->renderComponents[pin] = render;

        camera->fov = 45.0f;
        camera->aspectRatio = 16.0f / 9.0f;
        camera->nearPlane = 0.1f;
        camera->farPlane = 100.0f;
        camera->sensitivity = 0.5f;
        //camera.initialForward = { 0,0,-1,0 };
        app->cameraComponents[pin] = *camera;
    }


}

void LoaderRaw::loadBall(App* app, CameraComponent* camera) const{
    TransformComponent transform;
    PhysicsComponent physics;
    RenderComponent render;
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

    camera->fov = 60.0f;
    camera->aspectRatio = 16.0f / 9.0f;
    camera->nearPlane = 0.1f;
    camera->farPlane = 100.0f;
    camera->sensitivity = 0.1f;
    app->cameraComponents[ball] = *camera;
}


