#include "input_manager.h"

InputManager::InputManager(SystemManager* systemManager, EntityManager* entityManager)
    : systemManager(systemManager), entityManager(entityManager)
{}

InputManager::~InputManager()
{
    delete systemManager;
    delete entityManager;
}

bool InputManager::getInput(GLFWwindow* window, int hasPhysics)
{
    // Check if the ESC key is pressed to exit the update loop
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        return true;
    }

    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && hasPhysics) {
        physx::PxVec3 force(0, 0, 0.5);
        systemManager->motionSystem->applyForceToActor(entityManager->physicsComponents[1].rigidBody, force);
    }
    glfwPollEvents();
    return false;
}