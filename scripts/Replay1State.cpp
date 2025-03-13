#include "Replay1State.h"

void Replay1State::onLoad()
{

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
