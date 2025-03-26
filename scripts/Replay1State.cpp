#include "Replay1State.h"

void Replay1State::onLoad()
{
	setPositionByName("Ball1", ball_position_before_replay);
	setLinearVelocityByName("Ball1", ball_linear_velocity_before_replay);
	setAngularVelocityByName("Ball1", ball_angular_velocity_before_replay);
}

void Replay1State::running()
{
	if (wait("replay1", 3.0) || getAction("launch") || getActionOnController("launch", GLFW_JOYSTICK_1)) {
		releaseTimer("replay1");
		changeState(AllStates::INIT_TURN);
		setMainCameraByName("Camera");
	}
}

void Replay1State::onDestruct()
{
}
