#include "InitTurnState.h"

void InitTurnState::onLoad() {
	vec3 initial_position = { WIDTH_CAMERA, HEIGHT_CAMERA, drawRandomFloat(MIN_POSITION_Z + 0.1, MAX_POSITION_Z - 0.1) };
	setPositionByName("Camera", initial_position);

	vec3 initial_angle = { 3.13f, drawRandomFloat(-1.671, -1.471), 3.16f };
	setRotationEulerByName("Camera", initial_angle);


	update_preparation_position_ball(scriptManager);

	changeState(AllStates::PREPARATION);
}

void InitTurnState::running() {
	getPositionByName("Camera");
}

void InitTurnState::onDestruct() {

}