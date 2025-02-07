#pragma once
#include "../config.h"
#include "entity_manager.h"
#include "mesh_manager.h"
#include "system_manager.h"

class UI
{
    EntityManager* entityManager;
    MeshManager* meshManager;
    SystemManager* systemManager;

    //ImGui variables
    int selectedEntityID = -1;
    bool displaySceneEntities = true;
    int screenWidth = 1920;
    int screenHeight = 1080;

    //Inspector
    std::string remove_error_msg = "";
    char renameBuffer[128] = { 0 };
    bool displayInspector = false;

    // Guizmo
    int type_reference_frame = -1;
    bool grid_display = false;


    void CreateDockspace();

   
    int gizmo_type = -1;
    bool gizmo_world = true;
    physx::PxVec3 force = { 0.f, 0.f, 0.f };
    float targetMass;
    glm::vec2 targetSolverIteration;
    glm::vec2 targetDamping;
    float targetSleepT = 0.f;
    physx::PxVec3 targetCMass;


    // Imgui Create Entity Variables
        //Global
    /*
    char newEntityName[128] = "NewObject";
    static bool addTransform = false;
    static bool addPhysics = false;
    static bool addRender = false;
    static bool addLight = false;

    // Transform
    static glm::vec3 newPosition = { 0,0,0 };
    static glm::quat newEulers = { 0,0,0,0 };

    // Physics
    static int isStatic = 1;
    static int geometry = 0;
    static physx::PxVec3 boxSize = { 0, 0, 0 };
    static float sphereRadius = 0.0f;
    static std::string selectedModelName = "";
    static float newEntityStaticFriction = 0.5f;
    static float newEntityDynamicFriction = 0.5f;
    static float newEntityRestitution = 0.5f;
    static float newEntityMass = 1.f;
    static int newEntitySolverPosition = 4;
    static int newEntitySolverVelocity = 4;
    static float newEntityLinearDamping = 0.5f;
    static float newEntityAngularDamping = 0.5f;
    static float newEntitySleepT = 0.1f;

    static std::string selectedRModelName = "";
    static std::string selectedTexturesName = "";

    // Light
    static glm::vec3 newEntityColor = { 0,0,0 };
    static float newEntityIntensity = 0.f;

    static std::string error_msg = "";*/

    bool DecomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale);

public:
    UI(GLFWwindow* window, EntityManager* entityManager, MeshManager* meshManager, SystemManager*);
    ~UI();

    void update(int, int, GLuint, double);
    void displayEntityDetail();
    void displayNavBar();
    void displayFrameRate(double deltaTime);
    void displaySceneHierarchy();
    void displayCatalog();
    void displayScene(GLuint texture_id, double);
    void displaySettings();
};