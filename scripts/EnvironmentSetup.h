#pragma once
#define API_SCRIPT
#include <unordered_map>
#include <functional>
#include <memory>
#include "../src/controllers/script_manager.h"

#include <cmath>

// Déclarations pour les états
class StateOfGame;

#define HEIGHT_CAMERA 1.25
#define WIDTH_CAMERA -3.5

#define MIN_POSITION_Z 5.5
#define MAX_POSITION_Z 6.5

#define VIEW_CAMERA_CENTER_Y 0.7071
#define VIEW_CAMERA_LIMITS 0.06
#define PADDING_CAMERA_RADIAN 0.00872665f


extern glm::vec3 ball_position_before_replay;
extern physx::PxVec3 ball_linear_velocity_before_replay;
extern physx::PxVec3 ball_angular_velocity_before_replay;

extern int current_throw;
extern int current_turn;
extern std::vector<std::array<std::array<int, 3>, 10>> score_tab;
extern std::vector<int> non_modified_pin;

extern void update_preparation_position_ball(ScriptManager* scriptManager);


enum AllStates {
    INIT_TURN = 0,
    PREPARATION = 1,
    ROLLING = 2,
    CALCULATE_SCORE = 3,
    REPLAY_1 = 4
};

enum AllMasks {
    NONE_MASK,
    GAME_MASK = 1
};


//Enregistrer les masques
void saveMasks(ScriptManager* scriptManager);
// Enregistrer les états
void registerAllStates(ScriptManager* scriptManager);