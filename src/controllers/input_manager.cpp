#include "input_manager.h"

InputManager::InputManager(SystemManager* systemManager, EntityManager* entityManager)
    : systemManager(systemManager), entityManager(entityManager)
{}

InputManager::~InputManager()
{
    delete systemManager;
    delete entityManager;
}

/*
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
*/

bool InputManager::getInput_Press(int input) {
    return glfwGetKey(systemManager->cameraSystem->window, input) == GLFW_PRESS;
}

bool InputManager::getInput_Release(int input) {
    return glfwGetKey(systemManager->cameraSystem->window, input) == GLFW_RELEASE;
}

bool InputManager::getInput_PressOneTime(int input) {
    auto it = find(input_not_release_yet.begin(), input_not_release_yet.end(), input);
    if (it != input_not_release_yet.end()) {
        if (glfwGetKey(systemManager->cameraSystem->window, input) == GLFW_RELEASE){
            input_not_release_yet.erase(it);
        }
        return false;
    }
    else
    {
        if (glfwGetKey(systemManager->cameraSystem->window, input) == GLFW_PRESS) {
            input_not_release_yet.push_back(input);
            return true;
        }
        return false;
    }
}

bool InputManager::getInput_Press_For_Controller(int input, int controller)
{
    GLFWgamepadstate state;
    if (glfwGetGamepadState(controller, &state))
        return state.buttons[input] == GLFW_PRESS;
    else
        return false;
}

bool InputManager::getInput_Release_For_Controller(int input, int controller)
{
    GLFWgamepadstate state;
    if (glfwGetGamepadState(controller, &state))
        return state.buttons[input] == GLFW_RELEASE;
    else
        return false;
}

bool InputManager::getInput_PressOneTime_For_Controller(int input, int controller)
{
    GLFWgamepadstate state;
    if (glfwGetGamepadState(controller, &state)) {

        if (input_controller_not_release_yet.find(controller) == input_controller_not_release_yet.end()) {
            input_controller_not_release_yet[controller] = std::vector<int>();
        }

        std::vector<int> * vect = &(input_controller_not_release_yet[controller]);
        auto it = find(vect->begin(), vect->end(), input);
        if (it != vect->end()) {
            if (state.buttons[input] == GLFW_RELEASE) {
                vect->erase(it);
            }
            return false;
        }
        else
        {
            if (state.buttons[input] == GLFW_PRESS) {
                vect->push_back(input);
                return true;
            }
            return false;
        }

    }
    else
    {
        if (input_controller_not_release_yet.find(controller) != input_controller_not_release_yet.end())
            delete(&(input_controller_not_release_yet[controller]));
        return false;
    }


    



    auto it = find(input_not_release_yet.begin(), input_not_release_yet.end(), input);
    if (it != input_not_release_yet.end()) {
        if (glfwGetKey(systemManager->cameraSystem->window, input) == GLFW_RELEASE) {
            input_not_release_yet.erase(it);
        }
        return false;
    }
    else
    {
        if (glfwGetKey(systemManager->cameraSystem->window, input) == GLFW_PRESS) {
            input_not_release_yet.push_back(input);
            return true;
        }
        return false;
    }
}
