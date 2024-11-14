#include "config.h"
#include "controllers/app.h"

#include "components/camera_component.h"
#include "components/physics_component.h"
#include "components/render_component.h"
#include "components/transform_component.h"
#include "components/light_component.h"

#include "loaders/loader.h"
#include "loaders/loaderRaw.h"
#include "loaders/loaderJSON.h"

int main() {

		App* app = new App();

		TransformComponent transform;
		RenderComponent render;
		PhysicsComponent physics;
		LightComponent light;
		Loader* loader = new LoaderJSON();

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

		unsigned int lane = app->make_entity("Lane");
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

		unsigned int boule = app->make_entity("Boule");
		transform.position = { 0.0f, 0.105f, 3.0f};
		transform.eulers = {0.0f, 0.0f, 0.0f };
		app->transformComponents[boule] = transform;

		physics.velocity = { 0.f, 0.0f, 0.01f };
		physics.eulerVelocity = { 100, 0, 0};
		app->physicsComponents[boule] = physics;

		std::tuple<unsigned int, unsigned int> model = app->make_model("obj/servoskull/boule.obj");

		render.mesh = std::get<0>(model);
		render.indexCount = std::get<1>(model);
		render.material = app->make_texture("obj/bowling/textures/Bowling_Pack_Diffuse.png", false);
		app->renderComponents[boule] = render;

		// Chargement des quilles
		glm::vec3 first_pin = {0.f, 0.142f, 20.60475f}; 
		loader->loadQuilles(app);

		unsigned int cameraEntity = app->make_entity("Camera");
		transform.position = first_pin;
		transform.eulers = { 0.0f, 0.0f, 90.0f };
		app->transformComponents[cameraEntity] = transform;

		CameraComponent* camera = new CameraComponent();
		app->cameraComponent = camera;
		app->cameraID = cameraEntity;

		//First light : 
		unsigned int lightEntity1 = app->make_entity("First Light");
		transform.position = first_pin;
		transform.eulers = { 0.0f, 0.0f, 0.0f };
		app->transformComponents[lightEntity1] = transform;

		physics.velocity = { 0.001f, 0.01f, 0.001f };
		physics.eulerVelocity = { 52, 6, 50 };
		app->physicsComponents[lightEntity1] = physics;

		light.color = { 0.0f, 1.0f, 1.0f };
		light.intensity = 1.0f;
		app->lightComponents[lightEntity1] = light;

		std::tuple<unsigned int, unsigned int> defaultCube1 = app->make_cube_mesh({0.1f, 0.1f, 0.1f });
		render.mesh = std::get<0>(defaultCube1);
		render.indexCount = std::get<1>(defaultCube1);
		render.material = app->make_texture("tex/lightTex.png", false);
		app->renderComponents[lightEntity1] = render;


		//Second light: 
		unsigned int lightEntity2 = app->make_entity("Second Light");
		transform.position = { 0.0f, 4.0f, 4.0f };
		transform.eulers = { 0.0f, 0.0f, 0.0f };
		app->transformComponents[lightEntity2] = transform;

		light.color = { 1.0f, 1.0f, 1.0f };
		light.intensity = 1.0f;
		app->lightComponents[lightEntity2] = light;

		std::tuple<unsigned int, unsigned int> defaultCube2 = app->make_cube_mesh({ 0.1f, 0.1f, 0.1f });
		render.mesh = std::get<0>(defaultCube2);
		render.indexCount = std::get<1>(defaultCube2);
		render.material = app->make_texture("tex/lightTex.png", false);
		app->renderComponents[lightEntity2] = render;


		////glorious t34 :
		//unsigned int glaive = app->make_entity("T34");
		//transform.position = { 0.0f, 0.0f, 0.0f };
		//transform.eulers = { 270.0f, 0.0f, 0.0f };
		//app->transformComponents[glaive] = transform;

		//
		//std::tuple<unsigned int, unsigned int> t34 = app->make_model("obj/servoskull/t34.fbx");

		//render.mesh = std::get<0>(t34);
		//render.indexCount = std::get<1>(t34);
		//render.material = app->make_texture("obj/servoskull/text t34/Antracite_Base_color.png", false);
		//app->renderComponents[glaive] = render;


		app->set_up_opengl();
		app->make_systems();

		app->run();

		delete app;
		return 0;
}