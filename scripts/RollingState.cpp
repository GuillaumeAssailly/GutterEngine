#include "RollingState.h"

void RollingState::onLoad() {
	saved = false;
}

void RollingState::running() {

	glm::vec3 pos = getPositionByName("Ball1");

	if (wait("rolling", 10) || pos.z < 4.7f || pos.z > 7.3f) {
		releaseTimer("rolling");
		releaseTimer("finish");

		if (saved) {
			changeState(AllStates::CALCULATE_SCORE);
		}
		else {
			changeState(AllStates::INIT_TURN);
			setMainCameraByName("Camera");
		}
	}
	
	if (pos.x < -18.5f && !saved) {
		ball_position_before_replay = pos;
		ball_linear_velocity_before_replay = getLinearVelocityByName("Ball1");
		ball_angular_velocity_before_replay = getLinearVelocityByName("Ball1");
		saved = true;
	}

	if (saved && wait("finish", 4)) {
		releaseTimer("rolling");
		releaseTimer("finish");

		changeState(AllStates::CALCULATE_SCORE);
	}
	
}

void RollingState::onDestruct() {

}