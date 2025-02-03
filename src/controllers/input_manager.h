#pragma once
#include "../config.h"
#include "system_manager.h"
#include "entity_manager.h"

class InputManager
{
private:
    bool isCameraMoving;
    double mouse_x_ref, mouse_y_ref;
    SystemManager* systemManager;
    EntityManager* entityManager;
public:
    InputManager(SystemManager* systemManager, EntityManager* entityManager);
    ~InputManager();

    bool getInput(GLFWwindow* window, int hasPhysics);
};

