#pragma once
#include "../config.h"
#include "system_manager.h"
#include "entity_manager.h"

class InputManager
{
private:
    SystemManager* systemManager;
    EntityManager* entityManager;

    std::vector<int> input_not_release_yet;
    std::unordered_map<int, std::vector<int>> input_controller_not_release_yet;
public:
    InputManager(SystemManager* systemManager, EntityManager* entityManager);
    ~InputManager();

    friend class ScriptManager;

    //bool getInput(GLFWwindow* window, int hasPhysics);

    bool getInput_Press(int input);
    bool getInput_Release(int input);
    bool getInput_PressOneTime(int input);

    bool getInput_Press_For_Controller(int input, int controller);
    bool getInput_Release_For_Controller(int input, int controller);
    bool getInput_PressOneTime_For_Controller(int input, int controller);
};