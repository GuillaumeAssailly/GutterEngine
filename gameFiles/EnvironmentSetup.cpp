#include "EnvironmentSetup.h"

#include "InitTurnState.h"
#include "PreparationState.h"
#include "RollingState.h"
#include "CalculateScoreState.h"
#include "Replay1State.h"


#include "GAME_MASK.h"


std::vector<Save> ball_saves;
std::vector<std::vector<Save>> pin_saves(10);

int current_turn = 0;
int current_throw = 0;
std::vector<int> non_modified_pin = {1,2,3,4,5,6,7,8,9,10};
std::vector<std::array<std::array<int, 3>, 10>> score_tab = {
    std::array<std::array<int, 3>, 10>{
        std::array<int, 3>{-1, -1, -1},
        std::array<int, 3>{-1, -1, -1},
        std::array<int, 3>{-1, -1, -1},
        std::array<int, 3>{-1, -1, -1},
        std::array<int, 3>{-1, -1, -1},
        std::array<int, 3>{-1, -1, -1},
        std::array<int, 3>{-1, -1, -1},
        std::array<int, 3>{-1, -1, -1},
        std::array<int, 3>{-1, -1, -1},
        std::array<int, 3>{-1, -1, -1}
    }
};

void update_preparation_position_ball(GameManager* gameManager) {
	glm::vec3 look = getForwardMainCamera();
	glm::mat4 new_rotation = glm::rotate(glm::mat4(0.5f), -0.4f, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::vec4 v = new_rotation * glm::vec4(look, 0);

	glm::vec3 position_camera = getPositionByName("Camera");

	glm::vec3 position_ball = { position_camera.x + v.x, position_camera.y - 0.15, position_camera.z + v.z };
	setPositionByName("Ball1", position_ball);
}


void saveMasks(GameManager* gameManager) {
	addMask(AllMasks::GAME_MASK, new PREPARATION_MASK);
}


void registerAllStates(GameManager* gameManager) {
    registerStateFactory<InitTurnState>(AllStates::INIT_TURN, AllMasks::NONE , gameManager);
    registerStateFactory<PreparationState>(AllStates::PREPARATION, AllMasks::GAME_MASK ,gameManager);
	registerStateFactory<RollingState>(AllStates::ROLLING, AllMasks::GAME_MASK, gameManager);
	registerStateFactory<CalculateScoreState>(AllStates::CALCULATE_SCORE, AllMasks::NONE, gameManager);
    registerStateFactory<Replay1State>(AllStates::REPLAY_1, AllMasks::GAME_MASK , gameManager);
}