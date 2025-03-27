#include "game_manager.h"
#include "../../gameFiles/EnvironmentSetup.h"



std::unordered_map<int, StateFactory> stateFactoryMap;

GameManager::GameManager(InputManager* inputManager) : inputManager(inputManager)
{
    mainTime = static_cast<float>(glfwGetTime());
    CurrentState = 0;
}

void GameManager::initializeManager() {

    std::random_device rd;
    generator = std::mt19937(rd());

    addMask(0, new EMPTY_MASK);
    saveMasks(this);
    registerAllStates(this); // Enregistrer tous les états
    initializeStates();
    
    if (stateList.find(CurrentState) != stateList.end())
        stateList[CurrentState]->onLoad();
}

GameManager::~GameManager(){}

void GameManager::initializeStates() {
    for (const auto& factory : stateFactoryMap) {
        stateList.insert(std::make_pair(factory.first, factory.second(factory.first)));
    }
}


void GameManager::scriptManager_one_frame()
{
    stateList[CurrentState]->running();
}

void GameManager::addTime(float time)
{
	mainTime += time;
}

void GameManager::addMask(int id, Mask* mask)
{
    if (maskList.find(id) == maskList.end())
        maskList[id] = mask;
    else
        throw std::out_of_range("Error addMask : Mask with the given id " + std::to_string(id) + " already exist in maskList.");
}

void GameManager::changeState(int newState)
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

bool GameManager::wait(std::string key, float time)
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

void GameManager::releaseTimer(std::string key) {
    auto it = mapTimers.find(key);
    if (it != mapTimers.end()) mapTimers.erase(it);
}

// Position

glm::vec3 GameManager::getPositionByName(std::string name) {
    return inputManager->entityManager->getPositionByName(name);
}

glm::vec3 GameManager::getPositionByID(int id) {
    return inputManager->entityManager->getPositionByID(id);
}

void GameManager::setPositionByName(std::string name, glm::vec3 position) {
    inputManager->entityManager->setPositionByName(name, position);
}

void GameManager::setPositionByID(int id, glm::vec3 position) {
    inputManager->entityManager->setPositionByID(id, position);
}

// Rotation

glm::quat GameManager::getRotationQuaternionByName(std::string name) {
    return inputManager->entityManager->getRotationQuaternionByName(name);
}

glm::quat GameManager::getRotationQuaternionByID(int id) {
    return inputManager->entityManager->getRotationQuaternionByID(id);
}

void GameManager::setRotationQuaternionByName(std::string name, glm::quat rotation) {
    inputManager->entityManager->setRotationQuaternionByName(name, rotation);
}

void GameManager::setRotationQuaternionByID(int id, glm::quat rotation) {
    inputManager->entityManager->setRotationQuaternionByID(id, rotation);
}

glm::vec3 GameManager::getRotationEulerByName(std::string name) {
    return inputManager->entityManager->getRotationEulerByName(name);
}

glm::vec3 GameManager::getRotationEulerByID(int id) {
    return inputManager->entityManager->getRotationEulerByID(id);
}

void GameManager::setRotationEulerByName(std::string name, glm::vec3 eulers) {
    inputManager->entityManager->setRotationEulerByName(name, eulers);
}

void GameManager::setRotationEulerByID(int id, glm::vec3 eulers) {
    inputManager->entityManager->setRotationEulerByID(id, eulers);
}



// Physic

void GameManager::setForceByName(std::string name, physx::PxVec3 force)
{
    inputManager->entityManager->setForceByName(name, force);
}

void GameManager::setForceById(int id, physx::PxVec3 force)
{
    inputManager->entityManager->setForceById(id, force);
}

physx::PxVec3 GameManager::getLinearVelocityByName(std::string name)
{
    return inputManager->entityManager->getLinearVelocityByName(name);
}

physx::PxVec3 GameManager::getLinearVelocityById(int id)
{
    return inputManager->entityManager->getLinearVelocityById(id);
}

physx::PxVec3 GameManager::getAngularVelocityByName(std::string name)
{
    return inputManager->entityManager->getAngularVelocityByName(name);
}

physx::PxVec3 GameManager::getAngularVelocityById(int id)
{
    return inputManager->entityManager->getAngularVelocityById(id);
}

void GameManager::setLinearVelocityByName(std::string name, physx::PxVec3 velocity)
{
    inputManager->entityManager->setLinearVelocityByName(name, velocity);
}

void GameManager::setLinearVelocityById(int id, physx::PxVec3 velocity)
{
    inputManager->entityManager->setLinearVelocityById(id, velocity);
}

void GameManager::setAngularVelocityByName(std::string name, physx::PxVec3 velocity)
{
    inputManager->entityManager->setAngularVelocityByName(name, velocity);
}

void GameManager::setAngularVelocityById(int id, physx::PxVec3 velocity)
{
    inputManager->entityManager->setAngularVelocityById(id, velocity);
}


void GameManager::enablePhysicByName(std::string name)
{
    inputManager->entityManager->enablePhysicByName(name);
}

void GameManager::enablePhysicById(int id)
{
    inputManager->entityManager->enablePhysicById(id);
}

void GameManager::disablePhysicByName(std::string name)
{
    inputManager->entityManager->disablePhysicByName(name);
}

void GameManager::disablePhysicById(int id)
{
    inputManager->entityManager->disablePhysicById(id);
}



// Camera

std::vector<std::string> GameManager::getAllCamerasNames() {
    return inputManager->entityManager->getAllCamerasNames();
}

std::vector<int> GameManager::getAllCamerasID() {
    return inputManager->entityManager->getAllCamerasID();
}

std::string GameManager::getMainCameraName() {
    return inputManager->entityManager->getMainCameraName();
}

int GameManager::getMainCameraID() {
    return inputManager->entityManager->getMainCameraID();
}

void GameManager::setMainCameraByName(std::string name) {
    return inputManager->entityManager->setMainCameraByName(name);
}

void GameManager::setMainCameraByID(int id) {
    return inputManager->entityManager->setMainCameraByID(id);
}

glm::vec3 GameManager::getForwardMainCamera()
{
    return inputManager->systemManager->cameraSystem->getForward();
}

// Input

bool GameManager::getInput_Press(int input) {
    return inputManager->getInput_Press(input);
}

bool GameManager::getInput_Release(int input) {
    return inputManager->getInput_Release(input);
}

bool GameManager::getInput_PressOneTime(int input) {
    return inputManager->getInput_PressOneTime(input);
}

bool GameManager::getAction(std::string actionName)
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

bool GameManager::getActionOnController(std::string actionName, int numController)
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

int GameManager::drawRandomInt(int min, int max)
{
    std::uniform_int_distribution<int> distribution(min, max);
    return distribution(generator);
}

float GameManager::drawRandomFloat(float min, float max)
{
    std::uniform_real_distribution<float> distribution(min, max);
    return distribution(generator);
}






void GameManager::Mask::addAction(Action* action)
{
    if (listActions.find(action->name) == listActions.end())
        listActions[action->name] = action;
    else
        throw std::out_of_range("Error Mask::addAction : Action with the given name " + action->name + " already exist in this mask.");
}
