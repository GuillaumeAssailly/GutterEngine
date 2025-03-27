#pragma once
#define API_SCRIPT
#include <unordered_map>
#include <functional>
#include <memory>
#include "../src/controllers/game_manager.h"

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

struct Save {
    glm::vec3 position;
    glm::quat rotation;
};

extern std::vector<Save> ball_saves;
extern std::vector<std::vector<Save>> pin_saves;

extern int current_throw;
extern int current_turn;
extern std::vector<std::array<std::array<int, 3>, 10>> score_tab;
extern std::vector<int> non_modified_pin;

extern void update_preparation_position_ball(GameManager* gameManager);


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
void saveMasks(GameManager* gameManager);
// Enregistrer les états
void registerAllStates(GameManager* gameManager);