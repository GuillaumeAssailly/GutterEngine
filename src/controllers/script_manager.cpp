#include "script_manager.h"
#include "../../scripts/MainLoop.h"

ScriptManager::ScriptManager(InputManager* inputManager) : inputManager(inputManager)
{
	mainLoop = new MainLoop(this);
	mainTime = static_cast<float>(glfwGetTime());

    std::random_device rd;
    generator = std::mt19937(rd());
}

ScriptManager::~ScriptManager()
{
    delete mainLoop;
}

void ScriptManager::scriptManager_one_frame()
{
	mainLoop->mainLoop_one_frame();
}

void ScriptManager::addTime(float time)
{
	mainTime += time;
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
