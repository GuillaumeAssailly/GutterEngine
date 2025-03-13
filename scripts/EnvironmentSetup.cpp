#include "EnvironmentSetup.h"

#include "InitTurnState.h"
#include "PreparationState.h"
#include "Replay1State.h"

#include "GAME_MASK.h"


int GlobalVar;

void update_preparation_position_ball(ScriptManager* scriptManager) {
	glm::vec3 look = getForwardMainCamera();
	glm::mat4 new_rotation = glm::rotate(glm::mat4(0.5f), -0.4f, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::vec4 v = new_rotation * glm::vec4(look, 0);

	glm::vec3 position_camera = getPositionByName("Camera");

	glm::vec3 position_ball = { position_camera.x + v.x, position_camera.y - 0.15, position_camera.z + v.z };
	setPositionByName("Ball1", position_ball);
}


void saveMasks(ScriptManager* scriptManager) {
	addMask(AllMasks::GAME_MASK, new PREPARATION_MASK);
}


void registerAllStates(ScriptManager* scriptManager) {
    registerStateFactory<InitTurnState>(AllStates::INIT_TURN, AllMasks::NONE , scriptManager);
    registerStateFactory<PreparationState>(AllStates::PREPARATION, AllMasks::GAME_MASK ,scriptManager);
    registerStateFactory<Replay1State>(AllStates::REPLAY_1, AllMasks::GAME_MASK , scriptManager);
}