#include "loaderRaw.h"

void LoaderRaw::loadQuilles(App *app) const
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

	app->motionSystem->loadObjToPhysX("obj/convexMesh/quille.obj", meshes);
	app->addPhysicsModel("Quille", meshes);

	std::tuple<unsigned int, unsigned int> model = app->make_model("obj/servoskull/quille.obj");
	app->addRenderModel("Quille", std::get<0>(model), std::get<1>(model));

	for (int i = 0; i < 10; i++) {
		unsigned int quille = app->make_entity("Quille " + std::to_string(i));
		transform.position = vectors[i];

		transform.eulers = { 0.0f, 0.0f, 0.0f, 0.f };
		app->transformComponents[quille] = transform;

		physics.rigidBody = app->motionSystem->createDynamic(meshes, pinMaterial, transform.position, 1.5f, 0.01f, 0.2f, 0.3f, 255, 255);
		app->physicsComponents[quille] = physics;

		render.mesh = std::get<0>(model);
		render.indexCount = std::get<1>(model);
		app->renderComponents[quille] = render;
	}


}
