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

		app->set_up_opengl();
		app->make_systems();

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

		std::string inputFilePath = "obj/servoskull/quille.obj";
		std::string outputDir = "obj/servoskull/";

		std::cout << "Décomposition terminée. Les fichiers .obj convexes sont enregistrés dans le répertoire de sortie." << std::endl;

		unsigned int lane = app->make_entity();
		transform.position = { 0.f, 0.f, 0.f };
		transform.eulers = { 0.f, 0.f, 0.0f, 0.f};
		transform.size = { 1.0f, 0.168f, 18.0f };
		app->transformComponents[lane] = transform;

		glm::vec3 laneMaterial = { 0.5f, 0.5f, 0.6f };
		physx::PxBoxGeometry laneGeometry(physx::PxVec3(transform.size.x / 2.0f, transform.size.y / 2.0f, transform.size.z / 2.0f));

		app->createStatic(laneGeometry, laneMaterial, transform.position);

		std::tuple<unsigned int, unsigned int> laneModel = app->make_model("obj/servoskull/lane.obj");
		render.mesh = std::get<0>(laneModel);
		render.indexCount = std::get<1>(laneModel);
		render.material = app->make_texture("obj/servoskull/textures/lane.jpg", false);
		app->renderComponents[lane] = render;

		unsigned int boule = app->make_entity();
		transform.position = { 0.1f, 2.f, 0.050f};
		transform.eulers = {0.0f, 0.0f, 0.0f, 0.f};
		transform.size = { 0.21f, 0.21f, 0.21f };
		app->transformComponents[boule] = transform;

		physics.velocity = { 0.f, 0.0f, 0.f };
		physics.eulerVelocity = { 0, 0, 0};
		
		glm::vec3 ballMaterial = { 0.5f, 0.5f, 0.8f };
		physx::PxSphereGeometry ballGeometry(0.105f);

		physics.rigidBody  = app->createDynamic(ballGeometry, ballMaterial, transform.position, 6.8f);
		app->physicsComponents[boule] = physics;

		std::tuple<unsigned int, unsigned int> ballModel = app->make_model("obj/servoskull/boule.obj");
		 
		render.mesh = std::get<0>(ballModel);
		render.indexCount = std::get<1>(ballModel);
		render.material = app->make_texture("obj/bowling/textures/Bowling_Pack_Diffuse.png", false);
		app->renderComponents[boule] = render;

		
		glm::vec3 first_pin = { 0.f, 0.15f, 5.f };
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

		physx::PxConvexMesh* pinModelPhysics = app->make_physics_model("obj/servoskull/quilleLOD.obj");
		std::tuple<unsigned int, unsigned int> pinModel = app->make_model("obj/servoskull/quilleLOD.obj");
		float staticFriction = 0.8f;
		float dynamicFriction = 0.6f;
		float restitution = 0.2f;

		for (int i = 0; i < 10; i++) {
			unsigned int pin = app->make_entity();
			transform.position = vectors[i];
			transform.eulers = { 0.0f, 0.0f, 0.0f, 0.f };
			app->transformComponents[pin] = transform;

			physics.velocity = { 0.f, 0.0f, 0.f };
			physics.eulerVelocity = { 0, 0, 0 };

			glm::vec3 pinMaterial = { 0.5f, 0.5f, 0.2f };

			physics.rigidBody = app->createDynamic(physx::PxConvexMeshGeometry(pinModelPhysics, physx::PxMeshScale(), physx::PxConvexMeshGeometryFlag::eTIGHT_BOUNDS), pinMaterial, transform.position, 1.5f, 0.005f, 0.0f, 0.0f);
			app->physicsComponents[pin] = physics;


			render.mesh = std::get<0>(pinModel);
			render.indexCount = std::get<1>(pinModel);
			app->renderComponents[pin] = render;
			
		}

		unsigned int cameraEntity = app->make_entity();
		transform.position = { -3.0f, 1.0f, 0.0f };
		transform.eulers = { 0.0f, 0.0f, 0.0f, 0.f};
		app->transformComponents[cameraEntity] = transform;

		CameraComponent* camera = new CameraComponent();
		app->cameraComponent = camera;
		app->cameraID = cameraEntity;

		app->run();

		delete app;
		return 0;
}