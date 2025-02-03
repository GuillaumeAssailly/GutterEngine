#pragma once
#include "../config.h"

#include "imgui_manager.h"
#include "entity_manager.h"
#include "mesh_manager.h"
#include "system_manager.h"
#include "input_manager.h"

#include "../view/shader.h"


//Callbacks
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

class App {
private:
    EntityManager* entityManager;
    ImguiManager* imguiManager;
    MeshManager* meshManager;
    SystemManager* systemManager;
    InputManager* inputManager;

public:
    App();
    ~App();
    void run();

    void set_up_opengl();
    void loadModelsAndTextures();


    void loadEntities();
    int getEntityByName(std::string name) const;

private:
    void set_up_glfw();

    bool hasPhysics = true;
    GLFWwindow* window;

    unsigned int shader;
    unsigned int shadowShader;
    unsigned int depthMapDebugShader;

	float deltaTime = 0.0f;
	float lastFrame = 0.0f;
    float accumulatedTime = 0.0f;
};