#include "CalculateScoreState.h"



void CalculateScoreState::onLoad() {

	score_tab = gameManager->score_tab;

	for (int i = 1; i < 11; i++) {
		glm::quat rot = getRotationQuaternionByName("Pin" + std::to_string(i));
		if (rot.x > -0.05 && rot.x < 0.05 && rot.z > -0.05 && rot.z < 0.05) {
			non_modified_pin.push_back(i);
		}
	}

    if(current_throw == 0)
	    score_tab.at(gameManager->current_player)[current_turn][current_throw] = 10 - non_modified_pin.size();
    else {
        if(current_throw == 2 && (score_tab.at(gameManager->current_player)[current_turn][1] == 10 || score_tab.at(gameManager->current_player)[current_turn][0]+score_tab.at(gameManager->current_player)[current_turn][1] == 10))
            score_tab.at(gameManager->current_player)[current_turn][current_throw] = 10 - non_modified_pin.size();
        else
            score_tab.at(gameManager->current_player)[current_turn][current_throw] = 10 - score_tab.at(gameManager->current_player)[current_turn][current_throw - 1] - non_modified_pin.size();
    }

	current_throw++;

    gameManager->score_tab = score_tab;

	setMainCameraByName("Camera2");
	changeState(AllStates::REPLAY_1);
}

void CalculateScoreState::running() {

}

void CalculateScoreState::onDestruct() {

}