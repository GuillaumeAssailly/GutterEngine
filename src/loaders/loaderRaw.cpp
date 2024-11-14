#include "loaderRaw.h"

void LoaderRaw::loadQuilles(App *app) const
{
	TransformComponent transform;
	PhysicsComponent physics;
	RenderComponent render;

	glm::vec3 first_pin = { 0.f, 0.142f, 20.60475f };
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
		unsigned int quille = app->make_entity("Quille " + std::to_string(i));
		transform.position = vectors[i];

		transform.eulers = { 0, 0, 0 };
		app->transformComponents[quille] = transform;

		physics.velocity = { 0.0f, 0.0f, 0.0f };
		physics.eulerVelocity = { 0.0f, 0.f, 0.f };
		app->physicsComponents[quille] = physics;

		std::tuple<unsigned int, unsigned int> model = app->make_model("obj/servoskull/quille.obj");

		render.mesh = std::get<0>(model);
		render.indexCount = std::get<1>(model);
		app->renderComponents[quille] = render;
	}


}
