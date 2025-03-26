#include "InitTurnState.h"

void InitTurnState::onLoad() {

	if (current_throw == 1 && score_tab.at(0)[current_turn][0] == 10) {
		current_throw = 0;
		current_turn++;
		non_modified_pin = { 1,2,3,4,5,6,7,8,9,10 };
	}

	if (current_throw == 2) {
		current_throw = 0;
		current_turn++;
		non_modified_pin = { 1,2,3,4,5,6,7,8,9,10 };
	}


	disablePhysicByName("Ball1");

	vec3 initial_position = { WIDTH_CAMERA, HEIGHT_CAMERA, drawRandomFloat(MIN_POSITION_Z + 0.1, MAX_POSITION_Z - 0.1) };
	setPositionByName("Camera", initial_position);

	vec3 initial_angle = { 3.13f, drawRandomFloat(-1.671, -1.471), 3.16f };
	setRotationEulerByName("Camera", initial_angle);

	for (auto pin : non_modified_pin) {
		std::string name = ("Pin" + std::to_string(pin));
		glm::vec3 pos = first_pin + offsets[pin-1];
		setPositionByName(name, pos);
		glm::quat rot = { 1.0f, 0.0f, 0.0f, 0.f };
		setRotationQuaternionByName(name, rot);
		enablePhysicByName(name);
	}

	non_modified_pin = {};

	changeState(AllStates::PREPARATION);
}

void InitTurnState::running() {

}

void InitTurnState::onDestruct() {

}