#pragma once
#include "EnvironmentSetup.h"

class CalculateScoreState : public GameManager::StateOfGame {
public:

    CalculateScoreState(int id, int id_Mask, GameManager* sm) : StateOfGame(id, id_Mask, sm) {};
    void onLoad() override;
    void running() override;
    void onDestruct() override;
};