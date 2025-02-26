#pragma once
#include "../config.h"

#include "entity_manager.h"
#include "system_manager.h"
#include "mesh_manager.h"
#include "input_manager.h"
#include "UI.h"

#include "../view/shader.h"


//Callbacks
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

class App {
public:
    App();
    ~App();
    void run();

    const float window_width = 0;
    const float window_height = 0;
   
    void set_up_opengl();
    void loadModelsAndTextures();

    void loadEntities();
	EntityManager* getEntityManager();

private:
    void set_up_glfw();
    EntityManager* entityManager;
    SystemManager* systemManager;
    MeshManager* meshManager;
    InputManager* inputManager;
    UI* ui;

    bool hasPhysics = true;
    GLFWwindow* window;

    std::unordered_map<std::string, std::vector<physx::PxConvexMesh*>> physicsModels;
    std::unordered_map<std::string, std::pair<unsigned int, unsigned int>> renderModels;
    std::unordered_map<std::string, unsigned int> texturesList;
	std::unordered_map<std::string, unsigned int> normalMapsList;
	std::unordered_map<std::string, unsigned int> emissiveList;
    std::unordered_map<std::string, unsigned int> aoList;
	std::unordered_map<std::string, unsigned int> metalnessRoughnessList;

    std::vector<unsigned int> VAOs;
    std::vector<unsigned int> VBOs;
    std::vector<unsigned int> textures;

    unsigned int shader;
    unsigned int shadowShader;
    unsigned int depthMapDebugShader;
    unsigned int reflectionShader;

	float deltaTime = 0.0f;
	float lastFrame = 0.0f;
    float accumulatedTime = 0.0f;
    
};