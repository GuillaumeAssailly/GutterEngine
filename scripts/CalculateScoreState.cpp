#include "CalculateScoreState.h"

void CalculateScoreState::displayScore(int nb_players)
{
	std::cout << "\n\n\n---------------------------------------------------------------" << std::endl;
	std::cout << "|  1  |  2  |  3  |  4  |  5  |  6  |  7  |  8  |  9  |   10  |" << std::endl;
	for (int p = 0; p < nb_players; p++) {
		std::cout << "---------------------------------------------------------------" << std::endl;
		for (int i = 0; i < 10; i++) {
			std::cout << "| ";
			for (int j = 0; j < ((i == 9) ? 3 : 2); j++) {
				switch (score_tab.at(p)[i][j]) {
				case -1:
					std::cout << " ";
					break;
				case 0:
					std::cout << "_";
					break;
				case 10:
					if (j == 0) std::cout << "X";
					else if (j == 1) std::cout << "\\";
					else if (j == 2) {
						if (score_tab.at(p)[i][1] == 10) std::cout << "X";
						else std::cout << "\\";
					}
					break;
				default:
					if (j == 0) std::cout << score_tab.at(p)[i][j];
					if (j == 1) std::cout << score_tab.at(p)[i][j] - score_tab.at(p)[i][0];
					if (j == 2) {
						if (score_tab.at(p)[i][1] == 10) std::cout << score_tab.at(p)[i][j];
						else std::cout << score_tab.at(p)[i][j] - score_tab.at(p)[i][1];
					}
				}
				std::cout << " ";
			}
		}
		std::cout << "|" << std::endl;
	}
	std::cout << "---------------------------------------------------------------" << std::endl;
}

void CalculateScoreState::onLoad() {
	
	glm::vec3 new_pos = { 0.0f, 0.0f, 0.0f };
	glm::quat new_rot = { 0.0f, 1.f, -1.f, 0.f };

	for (int i = 1; i < 11; i++) {
		std::string current_pin = "Pin" + std::to_string(i);
		glm::quat rot = getRotationQuaternionByName(current_pin);
		if (rot.x < -0.05 || rot.x > 0.05 || rot.z < -0.05 || rot.z > 0.05) {
			disablePhysicByName(current_pin);
			setPositionByName(current_pin, new_pos);
			setRotationQuaternionByName(current_pin, new_rot);
		}
		else {
			non_modified_pin.push_back(i);
		}
	}

	score_tab.at(0)[current_turn][current_throw] = 10 - non_modified_pin.size();

	current_throw++;

	displayScore(1);

	setMainCameraByName("Camera2");
	changeState(AllStates::REPLAY_1);
}

void CalculateScoreState::running() {

}

void CalculateScoreState::onDestruct() {

}