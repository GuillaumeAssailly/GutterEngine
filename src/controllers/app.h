#pragma once
#include "../config.h"

#include "../components/camera_component.h"
#include "../components/physics_component.h"
#include "../components/render_component.h"
#include "../components/transform_component.h"

#include "../systems/camera_system.h"
#include "../systems/motion_system.h"
#include "../systems/render_system.h"

#include "../view/shader.h"


//Callbacks
void framebuffer_size_callback(GLFWwindow* window, int width, int height);


class App {
public:
    App();
    ~App();
    void run();
    unsigned int make_entity();
    unsigned int make_cube_mesh(glm::vec3 size);
    std::tuple<unsigned int, unsigned int, btConvexHullShape*>  make_model(const char *, bool = false);

    void make_object(std::tuple<unsigned int, unsigned int, btConvexHullShape*> model, unsigned int tex, glm::vec3 position, float mass = 0.f, float friction = 0.5, float restitution = 0.5, glm::vec4 eulers = {0.f, 0.f, 0.f, 0.f}, glm::vec3 inertia = { 0.f, 0.f, 0.f });

    unsigned int make_texture(const char* filename, const bool );
    void set_up_opengl();
    void make_systems();
    void add_rigid_body(btRigidBody* rigidBody);
    

    //Components
    std::unordered_map<unsigned int, TransformComponent> transformComponents;
    std::unordered_map<unsigned int, PhysicsComponent> physicsComponents;
    CameraComponent* cameraComponent;
    unsigned int cameraID;
    std::unordered_map<unsigned int, RenderComponent> renderComponents;

private:
    void set_up_glfw();

    unsigned int entity_count = 0;
    GLFWwindow* window;

    std::vector<unsigned int> VAOs;
    std::vector<unsigned int> VBOs;
    std::vector<unsigned int> textures;

    unsigned int shader;

	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

    //Systems
    MotionSystem* motionSystem;
    CameraSystem* cameraSystem;
    RenderSystem* renderSystem;

    
};