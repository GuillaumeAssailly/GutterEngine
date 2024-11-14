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
		//TODO : check this 
		app->set_up_opengl();
		app->make_systems();

		
	
		//boule
		unsigned int lane = app->make_entity("Lane");
		transform.position = { 0.f, 0.f, 9.f };
		transform.eulers = { 0.f, 0.f, 0.0f, 0.f};
		transform.size = { 1.0f, 0.168f, 18.0f };
		app->transformComponents[lane] = transform;

		glm::vec3 laneMaterial = { 0.05f, 0.05f, 0.0f };
		physx::PxBoxGeometry laneGeometry(physx::PxVec3(transform.size.x / 2.0f, transform.size.y / 2.0f, transform.size.z / 2.0f));

		app->motionSystem->createStatic(laneGeometry, laneMaterial, transform.position);

		std::tuple<unsigned int, unsigned int> laneModel = app->make_model("obj/servoskull/lane.obj");
		render.mesh = std::get<0>(laneModel);
		render.indexCount = std::get<1>(laneModel);
		render.material = app->make_texture("obj/servoskull/textures/lane.jpg", false);
		app->renderComponents[lane] = render;

		unsigned int boule = app->make_entity("Boule");
		transform.position = { 0.0f, 0.105f, 3.0f};
		transform.eulers = {0.0f, 0.0f, 0.0f , 0.f};
		app->transformComponents[boule] = transform;

		physics.velocity = { 0.f, 0.0f, 0.f };
		physics.eulerVelocity = { 0, 0, 0};
		
		glm::vec3 ballMaterial = { 0.05f, 0.05f, 0.2f };
		const physx::PxSphereGeometry ballGeometry(0.105f);

		physics.rigidBody  = app->motionSystem->createDynamic(ballGeometry, ballMaterial, transform.position, 6.8f);
		app->physicsComponents[boule] = physics;

		std::tuple<unsigned int, unsigned int> ballModel = app->make_model("obj/servoskull/boule.obj");
		 
		render.mesh = std::get<0>(ballModel);
		render.indexCount = std::get<1>(ballModel);
		render.material = app->make_texture("obj/bowling/textures/Bowling_Pack_Diffuse.png", false);
		app->renderComponents[boule] = render;

		
		glm::vec3 first_pin = { 0.f, 0.22f, 15.f };
		glm::vec3 vectors[10] = {
			first_pin,
			glm::vec3(first_pin.x + 0.1524f, first_pin.y, first_pin.z + 0.2635f),
			glm::vec3(first_pin.x - 0.1524f, first_pin.	y, first_pin.z + 0.2635f),
			glm::vec3(first_pin.x - 0.3048f, first_pin.y, first_pin.z + 0.527f),
			glm::vec3(first_pin.x, first_pin.y, first_pin.z + 0.527f),
			glm::vec3(first_pin.x + 0.3048f, first_pin.y, first_pin.z + 0.527f),
			glm::vec3(first_pin.x + 0.1524f, first_pin.y, first_pin.z + 0.7905f),
			glm::vec3(first_pin.x - 0.1524f, first_pin.y, first_pin.z + 0.7905f),
			glm::vec3(first_pin.x + 0.4572f, first_pin.y, first_pin.z + 0.7905f),
			glm::vec3(first_pin.x - 0.4572f, first_pin.y, first_pin.z + 0.7905f)
		};

		//app->motionSystem->concaveToConvex("obj/servoskull/quille.obj", "obj/convexMesh/", "quille");

		std::vector<physx::PxConvexMesh*> meshes;
		app->motionSystem->loadObjToPhysX("obj/convexMesh/quille.obj", meshes);

		std::tuple<unsigned int, unsigned int> pinModel = app->make_model("obj/servoskull/quille.obj");

		for (int i = 0; i < 10; i++) {
			unsigned int quille = app->make_entity("Quille " + std::to_string(i));
			transform.position = vectors[i];
			transform.eulers = { 0.0f, 0.0f, 0.0f, 0.f };
			app->transformComponents[quille] = transform;

			physics.velocity = { 0.f, 0.0f, 0.f };
			physics.eulerVelocity = { 0, 0, 0 };

			glm::vec3 pinMaterial = { 0.5f, 0.5f, 0.5f };

			physics.rigidBody = app->motionSystem->createDynamic(meshes, pinMaterial, transform.position, 1.5f, 0.04f, 0.2f, 0.3f);
			app->physicsComponents[quille] = physics;


			render.mesh = std::get<0>(pinModel);
			render.indexCount = std::get<1>(pinModel);
			app->renderComponents[quille] = render;
			
		}

		unsigned int cameraEntity = app->make_entity("Camera");
		transform.position = first_pin;
		transform.eulers = { 0.0f, 0.0f, 0.0f, 0.f};
		app->transformComponents[cameraEntity] = transform;

		CameraComponent* camera = new CameraComponent();
		app->cameraComponent = camera;
		app->cameraID = cameraEntity;

		//First light : 
		unsigned int lightEntity1 = app->make_entity("First Light");
		transform.position = first_pin;
		transform.eulers = { 0.0f, 0.0f, 0.0f, 0.f};
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
		transform.eulers = { 0.0f, 0.0f, 0.0f, 0.f};
		app->transformComponents[lightEntity2] = transform;

		light.color = { 1.0f, 1.0f, 1.0f };
		light.intensity = 1.0f;
		app->lightComponents[lightEntity2] = light;

		std::tuple<unsigned int, unsigned int> defaultCube2 = app->make_cube_mesh({ 0.1f, 0.1f, 0.1f });
		render.mesh = std::get<0>(defaultCube2);
		render.indexCount = std::get<1>(defaultCube2);
		render.material = app->make_texture("tex/lightTex.png", false);
		app->renderComponents[lightEntity2] = render;


		



		app->run();

		delete app;
		return 0;
}