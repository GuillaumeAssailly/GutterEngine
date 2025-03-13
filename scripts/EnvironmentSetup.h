#pragma once
#define API_SCRIPT
#include <unordered_map>
#include <functional>
#include <memory>
#include "../src/controllers/script_manager.h"

#include <cmath>

#define HEIGHT_CAMERA 1.25
#define WIDTH_CAMERA -3.5

#define MIN_POSITION_Z 5.5
#define MAX_POSITION_Z 6.5

#define VIEW_CAMERA_CENTER_Y 0.7071
#define VIEW_CAMERA_LIMITS 0.06
#define PADDING_CAMERA_RADIAN 0.00872665f


extern int GlobalVar;

extern void update_preparation_position_ball(ScriptManager* scriptManager);

// Déclarations pour les états
class StateOfGame;
class InitTurnState;
class PreparationState;
class Replay1State;

enum AllStates {
    INIT_TURN = 0,
    PREPARATION = 1,
    REPLAY_1 = 2
};

// Enregistrer les états
void registerAllStates(ScriptManager* scriptManager);