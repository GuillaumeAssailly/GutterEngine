#include "RollingState.h"

void RollingState::onLoad() {
	saved = false;
}

void RollingState::running() {

	glm::vec3 pos = getPositionByName("Ball1");

	if (wait("rolling", 10) || pos.z < 4.7f || pos.z > 7.3f) {
		releaseTimer("rolling");
		releaseTimer("finish");
		releaseTimer("save");

		if (saved) {
			changeState(AllStates::CALCULATE_SCORE);
		}
		else {
			changeState(AllStates::INIT_TURN);
			setMainCameraByName("Camera");
		}
	}
	
	if (pos.x < -14.5f && !saved) {
		saved = true;
	}

	if (saved && wait("save", 0.000001)) {

		Save currentState = {
			pos,
			getRotationQuaternionByName("Ball1")
		};
		ball_saves.push_back(currentState);

		for (int i = 0; i < 10; ++i) {
			std::string pinName = "Pin" + std::to_string(i + 1);
			glm::vec3 pin_pos = getPositionByName(pinName);
			Save pinState = {
				pin_pos,
				getRotationQuaternionByName(pinName)
			};
			pin_saves[i].push_back(pinState);
		}
	}

	if (saved && wait("finish", 4)) {
		releaseTimer("rolling");
		releaseTimer("finish");
		releaseTimer("save");

		changeState(AllStates::CALCULATE_SCORE);
	}
	
}

void RollingState::onDestruct() {

}