#pragma once
#include "../config.h"

#include "../components/camera_component.h"
#include "../components/physics_component.h"
#include "../components/render_component.h"
#include "../components/transform_component.h"

#include "../systems/camera_system.h"
#include "../systems/motion_system.h"
#include "../systems/render_system.h"
#include "../systems/line_system.h"

#include "../view/shader.h"
#include "../components/light_component.h"
#include "../systems/light_system.h"


//Callbacks
void framebuffer_size_callback(GLFWwindow* window, int width, int height);


class App {
public:
    App();
    ~App();
    void run();
    unsigned int make_entity(const std::string&);
    std::tuple<unsigned int, unsigned int> make_cube_mesh(glm::vec3 size);
    std::tuple<unsigned int, unsigned int>  make_model(const char *);

    unsigned int make_texture(const char* filename, const bool );
    void set_up_opengl();
    void make_systems();
    void addPhysicsModel(std::string, std::vector<physx::PxConvexMesh*>);
    void addRenderModel(std::string, unsigned int, unsigned int);
    void addTexture(std::string, unsigned int);

    //Components
	std::unordered_map<unsigned int, std::string> entityNames;
    std::unordered_map<unsigned int, TransformComponent> transformComponents;
    std::unordered_map<unsigned int, PhysicsComponent> physicsComponents;
    CameraComponent* cameraComponent;
    unsigned int cameraID;
    std::unordered_map<unsigned int, LightComponent> lightComponents;
    std::unordered_map<unsigned int, RenderComponent> renderComponents;
    

    MotionSystem* motionSystem;

private:
    void set_up_glfw();

    unsigned int entity_count = 0;
    bool hasPhysics = true;
    GLFWwindow* window;

    std::vector<std::pair<std::string, std::vector<physx::PxConvexMesh*>>> physicsModels;
    std::vector<std::tuple<std::string, unsigned int, unsigned int>> renderModels;
    std::vector<std::pair<std::string, unsigned int>> texturesList;

    std::vector<unsigned int> VAOs;
    std::vector<unsigned int> VBOs;
    std::vector<unsigned int> textures;

    unsigned int shader;

	float deltaTime = 0.0f;
	float lastFrame = 0.0f;
    float accumulatedTime = 0.0f;

    //Systems
    CameraSystem* cameraSystem;
    RenderSystem* renderSystem;
    LightSystem* lightSystem;
    LineSystem* lineSystem;
};