#include "TravelingState.h"

void TravelingState::onLoad() {
	glm::vec3 position = { -0.37f, 0.82f, 7.64f };
	setPositionByName("Camera3", position);
	glm:quat rotation = { 0.606061f, 0.0141105f, -0.795078f, 0.0185116f };
	setRotationQuaternionByName("Camera3", rotation);
	compteur = 0;
}

void TravelingState::running() {
	if (compteur <= 100 && wait("traveling", 0.01)) {
		glm::vec3 new_position = glm::vec3(-0.0239f, 0.02225f, -0.00653f ) + getPositionByName("Camera3");
		setPositionByName("Camera3", new_position);
		compteur++;
	}
	else if(compteur > 100) {
		if(wait("stare", 0.2)) {
			releaseTimer("traveling");
			releaseTimer("stare");
			changeState(AllStates::INIT_TURN);
			setMainCameraByName("Camera");
		}
	}
}

void TravelingState::onDestruct() {

}