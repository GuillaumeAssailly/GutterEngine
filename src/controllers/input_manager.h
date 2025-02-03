#pragma once
#include "../config.h"
#include "system_manager.h"
#include "entity_manager.h"

class InputManager
{
private:
    SystemManager* systemManager;
    EntityManager* entityManager;
public:
    InputManager(SystemManager* systemManager, EntityManager* entityManager);
    ~InputManager();

    bool getInput(GLFWwindow* window, int hasPhysics);
};