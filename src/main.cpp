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

		unsigned int lane = app->make_entity();
		transform.position = { 0.f, 0.f, 0.f };
		transform.eulers = { 0.f, 0.f, 0.0f, 0.f};
		transform.size = { 1.0f, 0.168f, 18.0f };
		app->transformComponents[lane] = transform;

		btCollisionShape* laneShape = new btBoxShape(btVector3(transform.size.x/2, transform.size.y / 2, transform.size.z/2));
		btDefaultMotionState* laneMotionState = new btDefaultMotionState(btTransform(
			btQuaternion(transform.eulers.x, transform.eulers.y, transform.eulers.z, 1),
			btVector3(transform.position.x, transform.position.y, transform.position.z)
		));
		btScalar laneMass = 0.f;
		btVector3 laneInertia(0, 0, 0);
		laneShape->calculateLocalInertia(laneMass, laneInertia);

		btRigidBody::btRigidBodyConstructionInfo laneRigidBodyCI(laneMass, laneMotionState, laneShape, laneInertia);
		laneRigidBodyCI.m_restitution = 0.8f; // Réglage de la restitution
		laneRigidBodyCI.m_friction = 0.5f; // Ajustez la friction
		physics.rigidBody = new btRigidBody(laneRigidBodyCI);
		app->add_rigid_body(physics.rigidBody);	
		physics.velocity = { 0.f, 0.0f, 0.f };
		physics.eulerVelocity = { 0, 0, 0 };
		app->physicsComponents[lane] = physics;

		std::tuple<unsigned int, unsigned int, btConvexHullShape*> laneModel = app->make_model("obj/servoskull/lane.obj");
		render.mesh = std::get<0>(laneModel);
		render.indexCount = std::get<1>(laneModel);
		render.material = app->make_texture("obj/servoskull/textures/lane.jpg", false);
		app->renderComponents[lane] = render;

		unsigned int boule = app->make_entity();
		transform.position = { 0.f, 10.f, 0.1f};
		transform.eulers = {0.0f, 0.0f, 0.0f, 0.f};
		transform.size = { 0.21f, 0.21f, 0.21f };
		app->transformComponents[boule] = transform;

		btCollisionShape* ballShape = new btSphereShape(0.105f);
		btDefaultMotionState* ballMotionState = new btDefaultMotionState(btTransform(
			btQuaternion(transform.eulers.x, transform.eulers.y, transform.eulers.z, 1),
			btVector3(transform.position.x, transform.position.y, transform.position.z)
		));
		btScalar ballMass = 6.8f;
		btVector3 ballInertia(0, 0, 0);
		ballShape->calculateLocalInertia(ballMass, ballInertia);

		btRigidBody::btRigidBodyConstructionInfo ballRigidBodyCI(ballMass, ballMotionState, ballShape, ballInertia);
		ballRigidBodyCI.m_restitution = 0.5f;
		ballRigidBodyCI.m_friction = 0.8f;
		physics.rigidBody = new btRigidBody(ballRigidBodyCI);
		app->add_rigid_body(physics.rigidBody);
		physics.velocity = { 0.f, 0.0f, 0.f };
		physics.eulerVelocity = { 0, 0, 0};
		app->physicsComponents[boule] = physics;

		std::tuple<unsigned int, unsigned int, btConvexHullShape*> ballModel = app->make_model("obj/servoskull/boule.obj");

		render.mesh = std::get<0>(ballModel);
		render.indexCount = std::get<1>(ballModel);
		render.material = app->make_texture("obj/bowling/textures/Bowling_Pack_Diffuse.png", false);
		app->renderComponents[boule] = render;

		glm::vec3 first_pin = { 0.f, 0.2f, 5.60475f };
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

		std::tuple<unsigned int, unsigned int, btConvexHullShape*> quilleModel = app->make_model("obj/servoskull/quille.obj", true);
		for (int i = 0; i < 10; i++) {
			app->make_object(quilleModel, render.material, vectors[i], 1.5f, 0.5f, 0.1f);
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