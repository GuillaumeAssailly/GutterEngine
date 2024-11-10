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
    std::tuple<unsigned int, unsigned int>  make_model(const char *);

    unsigned int make_texture(const char* filename, const bool );
    void set_up_opengl();
    void make_systems();
    physx::PxConvexMesh* make_physics_model(const char* filename);
    physx::PxRigidDynamic* createDynamic(const physx::PxGeometry& geometry, glm::vec3 material, glm::vec3 transform, float mass, float sleepT = 0.005f, float linearDamp = 0.0f, float angularDamp = 0.0f);
    void createStatic(const physx::PxGeometry& geometry, glm::vec3 material, glm::vec3 transform);
    void decomposeMeshWithVHACD(const std::string& inputFilePath, const std::string& outputDir, physx::PxPhysics* physics);

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