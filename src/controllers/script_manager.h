#pragma once
#include "../config.h"
#include <functional>
#include "input_manager.h"

using namespace glm;


class ScriptManager
{

	// State Engine

	struct Mask {

	};


	// API

	struct Timer {
		std::string name;
		float startTime;
	};
	std::unordered_map<std::string, Timer > mapTimers;


public:

	class StateOfGame {
	public:
		int id;
		int mask;
		ScriptManager* scriptManager;

		StateOfGame(int id, int id_Mask, ScriptManager* sm)
			: id(id), mask(id_Mask), scriptManager(sm) { }
		virtual void onLoad() = 0;
		virtual void running() = 0;
		virtual void onDestruct() = 0;
		virtual ~StateOfGame() = default;
	};


	ScriptManager(InputManager* inputManager);
	void initializeManager();
	~ScriptManager();
	void scriptManager_one_frame();
	void addTime(float time);

	void changeState(int newState);

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

	void setForceByName(std::string name, physx::PxVec3 force);
	void setForceById(int id, physx::PxVec3 force);

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
	InputManager* inputManager;
	float mainTime;

	std::mt19937 generator;

	int CurrentState;
	std::unordered_map<int, std::unique_ptr<StateOfGame>> stateList;

	void initializeStates();
};



// Typedef pour les fonctions d'usine
using StateFactory = std::function<std::unique_ptr<ScriptManager::StateOfGame>(int, int)>;

// Map globale pour enregistrer les fonctions d'usine
extern std::unordered_map<int, StateFactory> stateFactoryMap;

// Fonction pour enregistrer une fonction d'usine
template<typename T>
void registerStateFactory(int id, ScriptManager* scriptManager) {
	stateFactoryMap[id] = [scriptManager](int id, int id_Mask) -> std::unique_ptr<ScriptManager::StateOfGame> {
		return std::make_unique<T>(id, id_Mask, scriptManager);
		};
}



#ifdef API_SCRIPT
#define GLOBAL_SCRIPT_MANAGER scriptManager
#define wait(key, time) GLOBAL_SCRIPT_MANAGER->wait(key, time)
#define releaseTimer(key) GLOBAL_SCRIPT_MANAGER->releaseTimer(key)

#define changeState(newState) GLOBAL_SCRIPT_MANAGER->changeState(newState)

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

#define setForceByName(name, force) GLOBAL_SCRIPT_MANAGER->setForceByName(name, force)
#define setForceById(id, force) GLOBAL_SCRIPT_MANAGER->setForceById(id, force)

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

