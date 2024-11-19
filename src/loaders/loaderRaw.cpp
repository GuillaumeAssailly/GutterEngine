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


	for (int i = 0; i < 10; i++) {
		unsigned int pin = app->make_entity("Pin " + std::to_string(i));
		transform.position = vectors[i];
		transform.eulers = { 0.0f, 0.0f, 0.0f, 0.f };
		app->transformComponents[pin] = transform;

	}


}
