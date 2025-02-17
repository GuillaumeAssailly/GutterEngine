#pragma once
#include "../config.h"
#include "input_manager.h"

using namespace glm;

class MainLoop;

class ScriptManager
{
	struct Timer {
		std::string name;
		float startTime;
	};
	std::unordered_map<std::string, Timer > mapTimers;


public:
	ScriptManager(InputManager* inputManager);
	~ScriptManager();
	void scriptManager_one_frame();
	void addTime(float time);

	bool wait(std::string key, float time);
	void releaseTimer(std::string key);

	glm::vec3 getPositionByName(std::string name);
	glm::vec3 getPositionByID(int id);
	void setPositionByName(std::string name, glm::vec3 position);
	void setPositionByID(int id, glm::vec3 position);

	glm::quat getRotationQuaternionByName(std::string name);
	glm::quat getRotationQuaternionByID(int id);
	void setRotationQuaternionByName(std::string name, glm::quat rotation);
	void setRotationQuaternionByID(int id, glm::quat rotation);
	glm::vec3 getRotationEulerByName(std::string name);
	glm::vec3 getRotationEulerByID(int id);
	void setRotationEulerByName(std::string name, glm::vec3 eulers);
	void setRotationEulerByID(int id, glm::vec3 eulers);

	std::vector<std::string> getAllCamerasNames();
	std::vector<int> getAllCamerasID();
	std::string getMainCameraName();
	int getMainCameraID();
	void setMainCameraByName(std::string name);
	void setMainCameraByID(int id);
	glm::vec3 getForwardMainCamera();

	bool getInput_Press(int input);
	bool getInput_Release(int input);
	bool getInput_PressOneTime(int input);

	int drawRandomInt(int min, int max);
	float drawRandomFloat(float min, float max);


private:
	MainLoop* mainLoop;
	InputManager* inputManager;
	float mainTime;

	std::mt19937 generator;

friend class MainLoop;
};

#ifdef MAINLOOP_CPP
#define GLOBAL_SCRIPT_MANAGER scriptManager
#define wait(key, time) GLOBAL_SCRIPT_MANAGER->wait(key, time)
#define releaseTimer(key) GLOBAL_SCRIPT_MANAGER->releaseTimer(key)

#define getPositionByName(name) GLOBAL_SCRIPT_MANAGER->getPositionByName(name)
#define getPositionByID(id) GLOBAL_SCRIPT_MANAGER->getPositionByID(id)
#define setPositionByName(name, position) GLOBAL_SCRIPT_MANAGER->setPositionByName(name, position)
#define setPositionByID(id, position) GLOBAL_SCRIPT_MANAGER->setPositionByID(id, position)

#define getRotationQuaternionByName(name) GLOBAL_SCRIPT_MANAGER->getRotationQuaternionByName(name)
#define getRotationQuaternionByID(id) GLOBAL_SCRIPT_MANAGER->getRotationQuaternionByID(id)
#define setRotationQuaternionByName(name, rotation) GLOBAL_SCRIPT_MANAGER->setRotationQuaternionByName(name, rotation)
#define setRotationQuaternionByID(id, rotation) GLOBAL_SCRIPT_MANAGER->setRotationQuaternionByID(id, rotation)
#define getRotationEulerByName(name) GLOBAL_SCRIPT_MANAGER->getRotationEulerByName(name)
#define getRotationEulerByID(id) GLOBAL_SCRIPT_MANAGER->getRotationEulerByID(id)
#define setRotationEulerByName(name, eulers) GLOBAL_SCRIPT_MANAGER->setRotationEulerByName(name, eulers)
#define setRotationEulerByID(id, eulers) GLOBAL_SCRIPT_MANAGER->setRotationEulerByID(id, eulers)

#define getAllCamerasNames() GLOBAL_SCRIPT_MANAGER->getAllCamerasNames()
#define getAllCamerasID() GLOBAL_SCRIPT_MANAGER->getAllCamerasID()
#define getMainCameraName() GLOBAL_SCRIPT_MANAGER->getMainCameraName()
#define getMainCameraID() GLOBAL_SCRIPT_MANAGER->getMainCameraID()
#define setMainCameraByName(name) GLOBAL_SCRIPT_MANAGER->setMainCameraByName(name)
#define setMainCameraByID(id) GLOBAL_SCRIPT_MANAGER->setMainCameraByID(id)
#define getForwardMainCamera() GLOBAL_SCRIPT_MANAGER->getForwardMainCamera()

#define getInput_Press(input) GLOBAL_SCRIPT_MANAGER->getInput_Press(input)
#define getInput_Release(input) GLOBAL_SCRIPT_MANAGER->getInput_Release(input)
#define getInput_PressOneTime(input) GLOBAL_SCRIPT_MANAGER->getInput_PressOneTime(input)

#define drawRandomInt(min, max) GLOBAL_SCRIPT_MANAGER->drawRandomInt(min, max)
#define drawRandomFloat(min, max) GLOBAL_SCRIPT_MANAGER->drawRandomFloat(min, max)
#endif