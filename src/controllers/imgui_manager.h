#pragma once
#include "../config.h"
#include "../components/render_component.h"

class ImguiManager
{

    //ImGui variables
    unsigned int selectedEntityID = 0;
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
    ImguiManager();
    ~ImguiManager();

    void init(GLFWwindow* window);

    void displayRenderDetail(RenderComponent& renderComponent);

    void ShowStyleEditor(int entity_count, std::unordered_map<unsigned int, std::string> entityNames,
        std::unordered_map<std::string, unsigned int> texturesList, std::unordered_map<std::string, std::pair<unsigned int, unsigned int>> renderModels,
        std::unordered_map<unsigned int, std::list<RenderComponent>> renderComponents);

    void displayFrameRate(double deltaTime);
    int displaySceneHierarchy(int entity_count, std::unordered_map<unsigned int, std::string> entityNames,
        std::unordered_map<std::string, unsigned int> texturesList, std::unordered_map<std::string, std::pair<unsigned int, unsigned int>> renderModels,
        std::unordered_map<unsigned int, std::list<RenderComponent>> renderComponents);
};

