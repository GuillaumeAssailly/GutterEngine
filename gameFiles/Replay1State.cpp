#include "Replay1State.h"

void Replay1State::onLoad()
{
    replayIndex = 0;

    if (!ball_saves.empty()) {
        disablePhysicByName("Ball1");
        Save firstState = ball_saves.front();
        setPositionByName("Ball1", firstState.position);
        setRotationQuaternionByName("Ball1", firstState.rotation);
    }

    for (int i = 0; i < 10; ++i) {
        if (!pin_saves[i].empty()) {
            Save firstPinState = pin_saves[i].front();
            std::string pinName = "Pin" + std::to_string(i + 1);
            disablePhysicByName(pinName);
            setPositionByName(pinName, firstPinState.position);
            setRotationQuaternionByName(pinName, firstPinState.rotation);
        }
    }
}

void Replay1State::running()
{
    if (wait("load", 0.000001) && replayIndex < ball_saves.size()) {
        Save currentState = ball_saves[replayIndex];
        setPositionByName("Ball1", currentState.position);
        setRotationQuaternionByName("Ball1", currentState.rotation);


        for (int i = 0; i < 10; ++i) {
            if (replayIndex < pin_saves[i].size()) {
                Save pinState = pin_saves[i][replayIndex];
                std::string pinName = "Pin" + std::to_string(i + 1);
                setPositionByName(pinName, pinState.position);
                setRotationQuaternionByName(pinName, pinState.rotation);
            }
        }

        replayIndex++;
    }


	if (wait("replay1", 4.0) || getAction("launch") || getActionOnController("launch", gameManager->current_player)) {
		releaseTimer("replay1");
        releaseTimer("load");
		changeState(AllStates::TRAVELING);
		setMainCameraByName("Camera3");
	}
}

void Replay1State::onDestruct()
{
    ball_saves.clear();
    enablePhysicByName("Ball1");
    for (int i = 0; i < 10; ++i) {
        pin_saves[i].clear();
        enablePhysicByName("Pin" + std::to_string(i + 1));
    }
}
