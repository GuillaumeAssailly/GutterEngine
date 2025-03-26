#include "script_manager.h"
#include "../../scripts/EnvironmentSetup.h"



std::unordered_map<int, StateFactory> stateFactoryMap;

ScriptManager::ScriptManager(InputManager* inputManager) : inputManager(inputManager)
{
    mainTime = static_cast<float>(glfwGetTime());
    CurrentState = 0;
}

void ScriptManager::initializeManager() {

    std::random_device rd;
    generator = std::mt19937(rd());

    addMask(0, new EMPTY_MASK);
    saveMasks(this);
    registerAllStates(this); // Enregistrer tous les états
    initializeStates();
    
    if (stateList.find(CurrentState) != stateList.end())
        stateList[CurrentState]->onLoad();
}

ScriptManager::~ScriptManager(){}

void ScriptManager::initializeStates() {
    for (const auto& factory : stateFactoryMap) {
        stateList.insert(std::make_pair(factory.first, factory.second(factory.first)));
    }
}


void ScriptManager::scriptManager_one_frame()
{
    stateList[CurrentState]->running();
}

void ScriptManager::addTime(float time)
{
	mainTime += time;
}

void ScriptManager::addMask(int id, Mask* mask)
{
    if (maskList.find(id) == maskList.end())
        maskList[id] = mask;
    else
        throw std::out_of_range("Error addMask : Mask with the given id " + std::to_string(id) + " already exist in maskList.");
}

void ScriptManager::changeState(int newState)
{
    if (stateList.find(newState) != stateList.end()) {
        stateList[CurrentState]->onDestruct();
        CurrentState = newState;
        stateList[CurrentState]->onLoad();
    }
    else {
        throw std::out_of_range("Error changeState : State with the given id " + std::to_string(newState) + " doesn't exist.");
    }
    
}

bool ScriptManager::wait(std::string key, float time)
{
    auto it = mapTimers.find(key);

    if (it == mapTimers.end()) {
        float currentTime = mainTime;
        mapTimers[key] = { key, currentTime };
        return false;
    }
    else {
        if (mainTime >= it->second.startTime + time) {
            mapTimers.erase(it);
            return true;
        }
        else {
            return false;
        }

    }
}

void ScriptManager::releaseTimer(std::string key) {
    auto it = mapTimers.find(key);
    if (it != mapTimers.end()) mapTimers.erase(it);
}

// Position

glm::vec3 ScriptManager::getPositionByName(std::string name) {
    return inputManager->entityManager->getPositionByName(name);
}

glm::vec3 ScriptManager::getPositionByID(int id) {
    return inputManager->entityManager->getPositionByID(id);
}

void ScriptManager::setPositionByName(std::string name, glm::vec3 position) {
    inputManager->entityManager->setPositionByName(name, position);
}

void ScriptManager::setPositionByID(int id, glm::vec3 position) {
    inputManager->entityManager->setPositionByID(id, position);
}

// Rotation

glm::quat ScriptManager::getRotationQuaternionByName(std::string name) {
    return inputManager->entityManager->getRotationQuaternionByName(name);
}

glm::quat ScriptManager::getRotationQuaternionByID(int id) {
    return inputManager->entityManager->getRotationQuaternionByID(id);
}

void ScriptManager::setRotationQuaternionByName(std::string name, glm::quat rotation) {
    inputManager->entityManager->setRotationQuaternionByName(name, rotation);
}

void ScriptManager::setRotationQuaternionByID(int id, glm::quat rotation) {
    inputManager->entityManager->setRotationQuaternionByID(id, rotation);
}

glm::vec3 ScriptManager::getRotationEulerByName(std::string name) {
    return inputManager->entityManager->getRotationEulerByName(name);
}

glm::vec3 ScriptManager::getRotationEulerByID(int id) {
    return inputManager->entityManager->getRotationEulerByID(id);
}

void ScriptManager::setRotationEulerByName(std::string name, glm::vec3 eulers) {
    inputManager->entityManager->setRotationEulerByName(name, eulers);
}

void ScriptManager::setRotationEulerByID(int id, glm::vec3 eulers) {
    inputManager->entityManager->setRotationEulerByID(id, eulers);
}



// Physic

void ScriptManager::setForceByName(std::string name, physx::PxVec3 force)
{
    inputManager->entityManager->setForceByName(name, force);
}

void ScriptManager::setForceById(int id, physx::PxVec3 force)
{
    inputManager->entityManager->setForceById(id, force);
}

physx::PxVec3 ScriptManager::getLinearVelocityByName(std::string name)
{
    return inputManager->entityManager->getLinearVelocityByName(name);
}

physx::PxVec3 ScriptManager::getLinearVelocityById(int id)
{
    return inputManager->entityManager->getLinearVelocityById(id);
}

physx::PxVec3 ScriptManager::getAngularVelocityByName(std::string name)
{
    return inputManager->entityManager->getAngularVelocityByName(name);
}

physx::PxVec3 ScriptManager::getAngularVelocityById(int id)
{
    return inputManager->entityManager->getAngularVelocityById(id);
}

void ScriptManager::setLinearVelocityByName(std::string name, physx::PxVec3 velocity)
{
    inputManager->entityManager->setLinearVelocityByName(name, velocity);
}

void ScriptManager::setLinearVelocityById(int id, physx::PxVec3 velocity)
{
    inputManager->entityManager->setLinearVelocityById(id, velocity);
}

void ScriptManager::setAngularVelocityByName(std::string name, physx::PxVec3 velocity)
{
    inputManager->entityManager->setAngularVelocityByName(name, velocity);
}

void ScriptManager::setAngularVelocityById(int id, physx::PxVec3 velocity)
{
    inputManager->entityManager->setAngularVelocityById(id, velocity);
}


void ScriptManager::enablePhysicByName(std::string name)
{
    inputManager->entityManager->enablePhysicByName(name);
}

void ScriptManager::enablePhysicById(int id)
{
    inputManager->entityManager->enablePhysicById(id);
}

void ScriptManager::disablePhysicByName(std::string name)
{
    inputManager->entityManager->disablePhysicByName(name);
}

void ScriptManager::disablePhysicById(int id)
{
    inputManager->entityManager->disablePhysicById(id);
}



// Camera

std::vector<std::string> ScriptManager::getAllCamerasNames() {
    return inputManager->entityManager->getAllCamerasNames();
}

std::vector<int> ScriptManager::getAllCamerasID() {
    return inputManager->entityManager->getAllCamerasID();
}

std::string ScriptManager::getMainCameraName() {
    return inputManager->entityManager->getMainCameraName();
}

int ScriptManager::getMainCameraID() {
    return inputManager->entityManager->getMainCameraID();
}

void ScriptManager::setMainCameraByName(std::string name) {
    return inputManager->entityManager->setMainCameraByName(name);
}

void ScriptManager::setMainCameraByID(int id) {
    return inputManager->entityManager->setMainCameraByID(id);
}

glm::vec3 ScriptManager::getForwardMainCamera()
{
    return inputManager->systemManager->cameraSystem->getForward();
}

// Input

bool ScriptManager::getInput_Press(int input) {
    return inputManager->getInput_Press(input);
}

bool ScriptManager::getInput_Release(int input) {
    return inputManager->getInput_Release(input);
}

bool ScriptManager::getInput_PressOneTime(int input) {
    return inputManager->getInput_PressOneTime(input);
}

bool ScriptManager::getAction(std::string actionName)
{
    int id_mask = stateList[CurrentState]->mask;
    if(maskList[id_mask]->listActions.find(actionName) == maskList[id_mask]->listActions.end())
        throw std::out_of_range("Error getAction : Action with the name " + actionName + " doesn't exist.");
    Action* action = maskList[id_mask]->listActions[actionName];
    switch (action->inputType) {
        case InputType::PRESS:
            for (int input : action->inputList) {
                if (inputManager->getInput_Press(input))
                    return true;
            }
            return false;
        case InputType::RELEASE:
            for (int input : action->inputList) {
                if (inputManager->getInput_Release(input))
                    return true;
            }
            return false;
        case InputType::PRESS_ONE_TIME:
            for (int input : action->inputList) {
                if (inputManager->getInput_PressOneTime(input))
                    return true;
            }
            return false;
        default:
            throw std::out_of_range("Error getAction : InputType with the id " + std::to_string(action->inputType) + " is not define.");
    }
}

bool ScriptManager::getActionOnController(std::string actionName, int numController)
{
    GLFWgamepadstate state;
    if (glfwGetGamepadState(numController, &state)) {
        int id_mask = stateList[CurrentState]->mask;
        if (maskList[id_mask]->listActions.find(actionName) == maskList[id_mask]->listActions.end())
            throw std::out_of_range("Error getAction : Action with the name " + actionName + " doesn't exist.");
        Action* action = maskList[id_mask]->listActions[actionName];
        switch (action->inputType) {
        case InputType::PRESS:
            for (int input : action->inputControllerList) {
                if (inputManager->getInput_Press_For_Controller(input, numController))
                    return true;
            }
            return false;
        case InputType::RELEASE:
            for (int input : action->inputControllerList) {
                if (inputManager->getInput_Release_For_Controller(input, numController))
                    return true;
            }
            return false;
        case InputType::PRESS_ONE_TIME:
            for (int input : action->inputControllerList) {
                if (inputManager->getInput_PressOneTime_For_Controller(input, numController))
                    return true;
            }
            return false;
        default:
            throw std::out_of_range("Error getAction : InputType with the id " + std::to_string(action->inputType) + " is not define.");
        }
    }
    return false;
}



// Random

int ScriptManager::drawRandomInt(int min, int max)
{
    std::uniform_int_distribution<int> distribution(min, max);
    return distribution(generator);
}

float ScriptManager::drawRandomFloat(float min, float max)
{
    std::uniform_real_distribution<float> distribution(min, max);
    return distribution(generator);
}






void ScriptManager::Mask::addAction(Action* action)
{
    if (listActions.find(action->name) == listActions.end())
        listActions[action->name] = action;
    else
        throw std::out_of_range("Error Mask::addAction : Action with the given name " + action->name + " already exist in this mask.");
}
