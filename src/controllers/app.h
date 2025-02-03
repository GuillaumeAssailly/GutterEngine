#pragma once
#include "../config.h"

#include "entity_manager.h"
#include "system_manager.h"

#include "../view/shader.h"
#include "../components/light_component.h"


//Callbacks
void framebuffer_size_callback(GLFWwindow* window, int width, int height);



class App {
public:
    App();
    ~App();
    void run();
    
    std::pair<unsigned int, unsigned int> make_cube_mesh(glm::vec3 size);
    std::pair<unsigned int, unsigned int>  make_model(const char *);
    void loadGLTF(const char* filePath, const char * texDir, const int);


    unsigned int make_texture(const char* filename, const bool );
    void set_up_opengl();
    void loadModelsAndTextures();

    void loadEntities();
    MotionSystem * getMotionSystem();
    std::unordered_map<std::string, std::pair<unsigned int, unsigned int>> getRenderModels() const;
    std::unordered_map<std::string, unsigned int> getTexturesList() const;
    std::unordered_map<std::string, std::vector<physx::PxConvexMesh*>> getPhysicsModels() const;
    int getEntityByName(std::string name) const;

private:
    void set_up_glfw();
    EntityManager* entityManager;
    SystemManager* systemManager;

    // TODO: Place this function in a specific ImGui file
    bool DecomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale);
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
};