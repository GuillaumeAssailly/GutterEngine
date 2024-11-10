#include "config.h"
#include "controllers/app.h"

#include "components/camera_component.h"
#include "components/physics_component.h"
#include "components/render_component.h"
#include "components/transform_component.h"
#include "components/light_component.h"

int main() {

		App* app = new App();

		TransformComponent transform;
		RenderComponent render;
		PhysicsComponent physics;
		LightComponent light;

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
		render.normalMap = app->make_texture("obj/bowling/textures/Bowling_Pack_Normal.png", false);
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
			render.normalMap = app->renderComponents[boule].normalMap;
			app->renderComponents[quille] = render;
		}

		unsigned int cameraEntity = app->make_entity("Camera");
		transform.position = { 2.632f, 0.638f, -2.827f };
		transform.eulers = { 0.0f, -2.2f, 127.798f };
		app->transformComponents[cameraEntity] = transform;

		CameraComponent* camera = new CameraComponent();
		app->cameraComponent = camera;
		app->cameraID = cameraEntity;

		//First light : 
		unsigned int lightEntity1 = app->make_entity("First Light");
		transform.position = { 2.632f, 0.638f, -2.827f };
		transform.eulers = { 0.0f, -2.2f, 127.798f };
		app->transformComponents[lightEntity1] = transform;

		physics.velocity = { 0.000f, 0.00f, 0.000f };
		physics.eulerVelocity = { 0, 0, 0 };
		app->physicsComponents[lightEntity1] = physics;

		light.color = { 1.0f, 1.0f, 1.0f };
		light.intensity = 2.056f;
		light.isDirectional = true;
		light.direction = { -1.4f, -2.9f, 0.4f };
		app->lightComponents[lightEntity1] = light;

		std::tuple<unsigned int, unsigned int> defaultCube1 = app->make_cube_mesh({0.1f, 0.1f, 0.1f });
		//render.mesh = std::get<0>(defaultCube1);
		render.indexCount = std::get<1>(defaultCube1);
		render.material = app->make_texture("tex/lightTex.png", false);
		//app->renderComponents[lightEntity1] = render;


		//Second light: 
		//unsigned int lightEntity2 = app->make_entity("Second Light");
		//transform.position = { 0.0f, 4.0f, 4.0f };
		//transform.eulers = { 0.0f, 0.0f, 0.0f };
		//app->transformComponents[lightEntity2] = transform;

		//light.color = { 1.0f, 1.0f, 1.0f };
		//light.intensity = 1.0f;
		//app->lightComponents[lightEntity2] = light;

		//std::tuple<unsigned int, unsigned int> defaultCube2 = app->make_cube_mesh({ 0.1f, 0.1f, 0.1f });
		//render.mesh = std::get<0>(defaultCube2);
		//render.indexCount = std::get<1>(defaultCube2);
		//render.material = app->make_texture("tex/lightTex.png", false);
		//app->renderComponents[lightEntity2] = render;


		////glorious t34 :
		unsigned int glaive = app->make_entity("T34");
		transform.position = { 0.0f, 0.0f, 0.0f };
		transform.eulers = { 270.0f, 0.0f, 0.0f };
		app->transformComponents[glaive] = transform;

		
		std::tuple<unsigned int, unsigned int> t34 = app->make_model("obj/servoskull/t34.fbx");

		render.mesh = std::get<0>(t34);
		render.indexCount = std::get<1>(t34);
		render.material = app->make_texture("obj/servoskull/text t34/Antracite_Base_color.png", false);
		//render.normalMap = app->make_texture("obj/servoskull/text t34/Antracite_Normal_OpenGL.png", false);
		app->renderComponents[glaive] = render;
		/*
		unsigned int glaive = app->make_entity("Store");
		transform.position = { 0.0f, 0.0f, 0.0f };
		transform.eulers = { 0.0f, 0.0f, 0.0f };
		app->transformComponents[glaive] = transform;

		
		std::tuple<unsigned int, unsigned int> t34 = app->make_model("obj/cp/Store.obj");

		render.mesh = std::get<0>(t34);
		render.indexCount = std::get<1>(t34);
		render.material = app->make_texture("obj/cp/textures/scene_Default_BaseColor.png", false);
		render.normalMap = app->make_texture("obj/cp/textures/scene_Default_Normal.png", false);

		*/
		app->renderComponents[glaive] = render;

		unsigned int road = app->make_entity("Road");
		transform.position = { 0.0f, 0.0f, 0.0f };
		transform.eulers = { 0.0f, 0.0f, 0.0f };
		app->transformComponents[road] = transform;

		
		std::tuple<unsigned int, unsigned int> roadMesh = app->make_model("obj/cp/AsphaltRoad.obj");

		render.mesh = std::get<0>(roadMesh);
		render.indexCount = std::get<1>(roadMesh);
		render.material = app->make_texture("obj/cp/textures/2K-asphalt_1-diffuse.jpeg", false);
		app->renderComponents[road] = render;

		unsigned int glass = app->make_entity("Glass");
		transform.position = { 0.0f, 0.0f, 0.0f };
		transform.eulers = { 0.0f, 0.0f, 0.0f };
		app->transformComponents[glass] = transform;


		std::tuple<unsigned int, unsigned int> glassMesh = app->make_model("obj/cp/Glass.obj");

		render.mesh = std::get<0>(glassMesh);
		render.indexCount = std::get<1>(glassMesh);
		render.material = app->make_texture("obj/cp/textures/2K-asphalt_1-diffuse.jpeg", false);
		app->renderComponents[glass] = render;


		app->set_up_opengl();
		app->make_systems();

		app->run();

		delete app;
		return 0;
}