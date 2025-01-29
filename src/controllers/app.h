#pragma once
#include "../config.h"

#include "../components/camera_component.h"
#include "../components/physics_component.h"
#include "../components/static_physics_component.h"
#include "../components/render_component.h"
#include "../components/transform_component.h"

#include "../systems/camera_system.h"
#include "../systems/motion_system.h"
#include "../systems/render_system.h"
#include "../systems/line_system.h"

#include "../view/shader.h"
#include "../components/light_component.h"
#include "../systems/light_system.h"
#include "../systems/shadow_system.h"


//Callbacks
void framebuffer_size_callback(GLFWwindow* window, int width, int height);



class App {
public:
    App();
    ~App();
    void run();
    unsigned int make_entity(const std::string&);
    std::pair<unsigned int, unsigned int> make_cube_mesh(glm::vec3 size);
    std::pair<unsigned int, unsigned int>  make_model(const char *);
    void loadGLTF(const char* filePath, const char * texDir, const int);


    unsigned int make_texture(const char* filename, const bool );
	unsigned int make_normal(const char* filename, const bool flipTex);
    void set_up_opengl();
    void make_systems();
    void loadModelsAndTextures();

    void loadEntities();
    MotionSystem * getMotionSystem();
    std::unordered_map<std::string, std::pair<unsigned int, unsigned int>> getRenderModels() const;
    std::unordered_map<std::string, unsigned int> getTexturesList() const;
    std::unordered_map<std::string, std::vector<physx::PxConvexMesh*>> getPhysicsModels() const;
    int getEntityByName(std::string name) const;

    //Components
	std::unordered_map<unsigned int, std::string> entityNames;
    std::unordered_map<unsigned int, TransformComponent> transformComponents;
    std::unordered_map<unsigned int, PhysicsComponent> physicsComponents;
    std::unordered_map<unsigned int, StaticPhysicsComponent> staticPhysicsComponents;
    std::unordered_map<unsigned int, CameraComponent> cameraComponents;
    unsigned int cameraID;
    std::unordered_map<unsigned int, LightComponent> lightComponents;
    std::unordered_map<unsigned int, std::list<RenderComponent>> renderComponents;

private:
    void set_up_glfw();

    // TODO: Place this function in a specific ImGui file
    bool DecomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale);

    unsigned int entity_count = 0;
    bool hasPhysics = true;
    GLFWwindow* window;

    std::unordered_map<std::string, std::vector<physx::PxConvexMesh*>> physicsModels;
    std::unordered_map<std::string, std::pair<unsigned int, unsigned int>> renderModels;
    std::unordered_map<std::string, unsigned int> texturesList;
	std::unordered_map<std::string, unsigned int> normalMapsList;

    std::vector<unsigned int> VAOs;
    std::vector<unsigned int> VBOs;
    std::vector<unsigned int> textures;

    unsigned int shader;
    unsigned int shadowShader;
    unsigned int depthMapDebugShader;

	float deltaTime = 0.0f;
	float lastFrame = 0.0f;
    float accumulatedTime = 0.0f;

    //Systems
    CameraSystem* cameraSystem;
    MotionSystem* motionSystem;
    RenderSystem* renderSystem;
    LightSystem* lightSystem;
    LineSystem* lineSystem;
	ShadowSystem* shadowSystem;


    

    
};