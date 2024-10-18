#include "config.h"
#include "controllers/app.h"

#include "components/camera_component.h"
#include "components/physics_component.h"
#include "components/render_component.h"
#include "components/transform_component.h"

int main() {

		App* app = new App();

		TransformComponent transform;
		RenderComponent render;
		PhysicsComponent physics;

		////BASE CUBE : 
		/*unsigned int cubeEntity = app->make_entity();
		transform.position = { 0.0f, 0.0f, 0.0f };
		transform.eulers = { 0.0f, 0.0f, 0.0f };
		app->transformComponents[cubeEntity] = transform;

		physics.velocity = { 0.0f, 0.0f, 0.0f };
		physics.eulerVelocity = { 0.0f, 0.0f, 0.0f };
		app->physicsComponents[cubeEntity] = physics;

		render.mesh = app->make_cube_mesh({ 0.25 , 0.25, 0.25 });
		
		render.material = app->make_texture("tex/basetex.png", false);
		app->renderComponents[cubeEntity] = render;*/
	
		//boule

		unsigned int lane = app->make_entity();
		transform.position = { 0.0f, -0.084f, 15.0f };
		transform.eulers = { 0.f, 0.f, 0.0f };
		app->transformComponents[lane] = transform;

		physics.velocity = { 0.f, 0.0f, 0.f };
		physics.eulerVelocity = { 0, 0, 0 };
		app->physicsComponents[lane] = physics;

		std::tuple<unsigned int, unsigned int> laneModel = app->make_model("obj/servoskull/lane.obj");
		render.mesh = std::get<0>(laneModel);
		render.indexCount = std::get<1>(laneModel);
		render.material = app->make_texture("obj/servoskull/textures/lane.jpg", false);
		app->renderComponents[lane] = render;

		unsigned int boule = app->make_entity();
		transform.position = { 0.0f, 0.105f, 3.0f};
		transform.eulers = {0.0f, 0.0f, 0.0f };
		app->transformComponents[boule] = transform;

		physics.velocity = { 0.f, 0.0f, 1.f };
		physics.eulerVelocity = { 100, 0, 0};
		app->physicsComponents[boule] = physics;

		std::tuple<unsigned int, unsigned int> model = app->make_model("obj/servoskull/boule.obj");

		render.mesh = std::get<0>(model);
		render.indexCount = std::get<1>(model);
		render.material = app->make_texture("obj/bowling/textures/Bowling_Pack_Diffuse.png", false);
		app->renderComponents[boule] = render;

		glm::vec3 first_pin = { 0.f, 0.142f, 20.60475f };
		glm::vec3 vectors[10] = {
			first_pin,
			glm::vec3(first_pin.x + 0.1524f, first_pin.y, first_pin.z + 0.2635f),
			glm::vec3(first_pin.x - 0.1524f, first_pin.y, first_pin.z + 0.2635f),
			glm::vec3(first_pin.x - 0.3048f, first_pin.y, first_pin.z + 0.527f),
			glm::vec3(first_pin.x, first_pin.y, first_pin.z + 0.527f),
			glm::vec3(first_pin.x + 0.3048f, first_pin.y, first_pin.z + 0.527f),
			glm::vec3(first_pin.x + 0.1524f, first_pin.y, first_pin.z + 0.7905f),
			glm::vec3(first_pin.x - 0.1524f, first_pin.y, first_pin.z + 0.7905f),
			glm::vec3(first_pin.x + 0.4572f, first_pin.y, first_pin.z + 0.7905f),
			glm::vec3(first_pin.x - 0.4572f, first_pin.y, first_pin.z + 0.7905f)
		};

		for (int i = 0; i < 10; i++) {
			unsigned int quille = app->make_entity();
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

		unsigned int cameraEntity = app->make_entity();
		transform.position = { 0.0f, 0.0f, 0.0f };
		transform.eulers = { 0.0f, 0.0f, 0.0f };
		app->transformComponents[cameraEntity] = transform;

		CameraComponent* camera = new CameraComponent();
		app->cameraComponent = camera;
		app->cameraID = cameraEntity;

		app->set_up_opengl();
		app->make_systems();

		app->run();

		delete app;
		return 0;
}