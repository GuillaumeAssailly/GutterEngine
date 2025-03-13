#include "Replay1State.h"

void Replay1State::onLoad()
{

}

void Replay1State::running()
{
	if (wait("replay1", 3.0) || getInput_PressOneTime(GLFW_KEY_SPACE)) {
		releaseTimer("replay1");
		changeState(AllStates::INIT_TURN);
		setMainCameraByName("Camera");
	}
}

void Replay1State::onDestruct()
{
}
