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
	/*	unsigned int cubeEntity = app->make_entity();
		transform.position = { 3.0f, 0.0f, 0.25f };
		transform.eulers = { 0.0f, 0.0f, 0.0f };
		app->transformComponents[cubeEntity] = transform;

		physics.velocity = { 0.0f, 0.0f, 0.0f };
		physics.eulerVelocity = { 0.0f, 0.0f, 10.0f };
		app->physicsComponents[cubeEntity] = physics;

		render.mesh = app->make_cube_mesh({ 0.25f, 0.25f, 0.25f });
		
		render.material = app->make_texture("tex/basetex.png");
		app->renderComponents[cubeEntity] = render;*/
	
		//boule
		unsigned int boule = app->make_entity();
		transform.position = { 0.0f, 2.0f, 2.0f};
		transform.eulers = { 100.0f, 100.0f, 100.0f };
		app->transformComponents[boule] = transform;

		physics.velocity = { 0.0f, 0.0f, 0.0f };
		physics.eulerVelocity = { 0.0f, 0.0f, 0.0f };
		app->physicsComponents[boule] = physics;

		std::tuple<unsigned int, unsigned int> model = app->make_model("obj/servoskull/boule.obj");

		render.mesh = std::get<0>(model);
		render.indexCount = std::get<1>(model);
		render.material = app->make_texture("obj/bowling/textures/Bowling_Pack_Diffuse.png", false);
		app->renderComponents[boule] = render;

		for (int i = 0; i < 10; i++) {
			unsigned int quille = app->make_entity();
			transform.position = {0.0f, 0.0f,(float)i };
			transform.eulers = { 100.0f, 100.0f, 100.0f };
			app->transformComponents[quille] = transform;

			physics.velocity = { 0.0f, 0.0f, 0.0f };
			physics.eulerVelocity = { 0.0f, 0.0f, 0.0f };
			app->physicsComponents[quille] = physics;

			std::tuple<unsigned int, unsigned int> model = app->make_model("obj/servoskull/quille.obj");

			render.mesh = std::get<0>(model);
			render.indexCount = std::get<1>(model);
			app->renderComponents[quille] = render;
		}

	unsigned int cameraEntity = app->make_entity();
	transform.position = { 0.0f, 0.0f, 1.0f };
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