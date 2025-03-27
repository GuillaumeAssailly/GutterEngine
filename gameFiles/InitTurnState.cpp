#include "InitTurnState.h"

void InitTurnState::onLoad() {

	if (current_throw >= 1 && score_tab.at(0)[current_turn][current_throw-1] == 10) {
		if (current_turn == 9) {
			non_modified_pin = { 1,2,3,4,5,6,7,8,9,10 };
		}
		else {
			current_throw = 0;
			current_turn++;
			non_modified_pin = { 1,2,3,4,5,6,7,8,9,10 };
		}
	}

	if (current_turn != 9 && current_throw == 2) {
		current_throw = 0;
		current_turn++;
		non_modified_pin = { 1,2,3,4,5,6,7,8,9,10 };
	}
	else if (current_turn == 9 && score_tab.at(0)[current_turn][1] && score_tab.at(0)[current_turn][0] == 10) {
		non_modified_pin = { 1,2,3,4,5,6,7,8,9,10 };
	}
	else if (current_turn == 9 && score_tab.at(0)[current_turn][1] == 10) {
		non_modified_pin = { 1,2,3,4,5,6,7,8,9,10 };
	}
	else if (current_turn == 9 && current_throw == 3) {
		std::cout << "\n\nFin du jeu !\n\n" << std::endl;
		current_throw = 0;
		current_turn = 0;
		non_modified_pin = { 1,2,3,4,5,6,7,8,9,10 };

		score_tab = {
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
	}


	disablePhysicByName("Ball1");

	vec3 initial_position = { WIDTH_CAMERA, HEIGHT_CAMERA, drawRandomFloat(MIN_POSITION_Z + 0.1, MAX_POSITION_Z - 0.1) };
	setPositionByName("Camera", initial_position);

	vec3 initial_angle = { 3.13f, drawRandomFloat(-1.671, -1.471), 3.16f };
	setRotationEulerByName("Camera", initial_angle);

	glm::vec3 new_pos = { 1.0f, 1.0f, 1.0f };
	glm::quat new_rot = glm::normalize(glm::quat( 0.0f, 0.5f, 0.f, 0.5f ));
	for (int i = 1; i < 11; i++) {
		std::string current_pin = "Pin" + std::to_string(i);
		if (std::find(non_modified_pin.begin(), non_modified_pin.end(), i) == non_modified_pin.end()) {
			setPositionByName(current_pin, new_pos);
			setRotationQuaternionByName(current_pin, new_rot);
			disablePhysicByName(current_pin);
		}
		else
		{
			glm::vec3 pos = first_pin + offsets[i - 1];
			setPositionByName(current_pin, pos);
			glm::quat rot = { 1.0f, 0.0f, 0.0f, 0.f };
			setRotationQuaternionByName(current_pin, rot);
			enablePhysicByName(current_pin);
		}
	}

	non_modified_pin = {};

	changeState(AllStates::PREPARATION);
}

void InitTurnState::running() {

}

void InitTurnState::onDestruct() {

}