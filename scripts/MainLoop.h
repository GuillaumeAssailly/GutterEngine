#pragma once
#include "../src/config.h"
#include <cmath>

class ScriptManager;

#define HEIGHT_CAMERA 1.25
#define WIDTH_CAMERA -3.5

#define MIN_POSITION_Z 5.5
#define MAX_POSITION_Z 6.5

#define VIEW_CAMERA_CENTER_Y 0.7071
#define VIEW_CAMERA_LIMITS 0.06
#define PADDING_CAMERA_RADIAN 0.00872665f


class MainLoop
{

	enum GameStateEnum {
		INIT_TURN = 0,
		PREPARATION = 1,
		ROLLING = 2,
		REPLAY_1 = 3,
		REPLAY_2 = 4,
		SCORE = 5
	};

public:
	MainLoop(ScriptManager* scriptManager);
	void mainLoop_one_frame();
private:
	void update_preparation_position_ball();

	ScriptManager* scriptManager;

	int current_game_state;
};