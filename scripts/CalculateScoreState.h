#pragma once
#include "EnvironmentSetup.h"

class CalculateScoreState : public ScriptManager::StateOfGame {
public:
    void displayScore(int nb_players);

    CalculateScoreState(int id, int id_Mask, ScriptManager* sm) : StateOfGame(id, id_Mask, sm) {};
    void onLoad() override;
    void running() override;
    void onDestruct() override;
};